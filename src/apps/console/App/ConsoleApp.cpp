/*
  ==============================================================================
	Module:         ConsoleApp
	Description:    Interactive console front end for the chess engine
  ==============================================================================
*/

#include "ConsoleApp.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "CommandParser.h"
#include "GameSetup.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif


using namespace std::chrono_literals;

namespace
{

std::string stripCheckSuffix(std::string s)
{
	while (!s.empty() && (s.back() == '+' || s.back() == '#'))
		s.pop_back();
	return s;
}

std::string toLowerAscii(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

// Colour is irrelevant to move submission, mirrors CommandParser::parsePromotion.
PieceType promotionFromOffset(int offset)
{
	switch (offset)
	{
	case 0: return PieceType::WKnight;
	case 1: return PieceType::WBishop;
	case 2: return PieceType::WRook;
	default: return PieceType::WQueen;
	}
}

} // namespace


ConsoleApp::ConsoleApp() : mView(mEngine) {}


ConsoleApp::~ConsoleApp()
{
	mEngine.shutDown();
}


//=========================================================================
// Setup
//=========================================================================

void ConsoleApp::configureConsole()
{
#if defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
#endif
}


EngineSettings ConsoleApp::defaultSettings()
{
	EngineSettings settings;
	settings.playerName = "Console Player";

#if defined(_WIN32)
	if (const char *localAppData = std::getenv("LOCALAPPDATA"))
		settings.logFolder = std::filesystem::path(localAppData) / "ChessEngine";
#else
	if (const char *home = std::getenv("HOME"))
		settings.logFolder = std::filesystem::path(home) / ".chessengine";
#endif

	return settings;
}


int ConsoleApp::run()
{
	configureConsole();
	mView.setUnicode(true);
	mView.printBanner();

	if (!mEngine.init(defaultSettings()))
	{
		mView.printError("The engine failed to start.");
		return 1;
	}

	if (!startNewGame())
		return 0; // input ended during setup

	mView.printHelp();

	while (mRunning)
	{
		if (mState == GameState::PawnPromotion)
		{
			promptPromotion();
			continue;
		}

		if (!canPrompt())
		{
			// The computer is thinking - keep consuming until it plays.
			pumpUntilIdle(200ms);
			continue;
		}

		promptCommand();
	}

	mEngine.shutDown();
	printf("\n  Thanks for playing.\n\n");
	return 0;
}


bool ConsoleApp::startNewGame()
{
	auto config = GameSetup::run();

	if (!config)
		return false;

	mMode = config->mode;
	mMoveLog.clear();
	mMover		= Side::White;
	mBoardDirty = true;

	mEngine.startGame(*config);
	pumpUntilIdle(2000ms);

	return true;
}


//=========================================================================
// Event Pump
//=========================================================================

void ConsoleApp::pumpUntilIdle(std::chrono::milliseconds firstWait)
{
	engine::EngineEvent event;

	if (!mEngine.events().waitAndPop(event, firstWait))
		return;

	handleEvent(event);

	// A single engine action publishes a burst of events. Keep taking them until
	// the queue stays empty for a moment, so we never prompt mid-burst.
	while (mEngine.events().waitAndPop(event, 50ms))
	{
		handleEvent(event);
	}
}


void ConsoleApp::handleEvent(const engine::EngineEvent &event)
{
	std::visit(
		[this](const auto &e)
		{
			using T = std::decay_t<decltype(e)>;

			if constexpr (std::is_same_v<T, engine::GameStateChanged>)
			{
				mState = e.state;
			}
			else if constexpr (std::is_same_v<T, engine::PlayerChanged>)
			{
				mMover = e.side;
			}
			else if constexpr (std::is_same_v<T, engine::MoveExecuted>)
			{
				mView.printMovePlayed(e, mMover);
				mMoveLog.push_back(e.notation);
			}
			else if constexpr (std::is_same_v<T, engine::MoveUndone>)
			{
				if (!mMoveLog.empty())
					mMoveLog.pop_back();

				mView.printMessage("Move taken back.");
			}
			else if constexpr (std::is_same_v<T, engine::BoardStateChanged>)
			{
				mBoardDirty = true;
			}
			else if constexpr (std::is_same_v<T, engine::PieceCaptured>)
			{
				mView.printCapture(e);
			}
			else if constexpr (std::is_same_v<T, engine::GameEnded>)
			{
				mBoardDirty = true;
				mView.printGameEnded(e);
			}
		},
		event);
}


//=========================================================================
// Turn Handling
//=========================================================================

bool ConsoleApp::canPrompt() const
{
	// GameOver still accepts input so the player can start over or leave.
	return mState == GameState::WaitingForInput || mState == GameState::WaitingForTarget || mState == GameState::GameOver;
}


void ConsoleApp::promptPromotion()
{
	std::string line;

	while (true)
	{
		if (!readLine("  Promote to (q/r/b/n): ", line))
		{
			mRunning = false;
			return;
		}

		if (line.empty())
			continue;

		if (auto piece = CommandParser::parsePromotion(line[0]))
		{
			mEngine.onPromotionChosen(*piece);
			pumpUntilIdle(2000ms);
			return;
		}

		mView.printError("Pick one of q, r, b or n.");
	}
}


void ConsoleApp::promptCommand()
{
	if (mBoardDirty)
	{
		mView.printBoard();

		if (mState != GameState::GameOver)
			mView.printStatus();

		mBoardDirty = false;
	}

	const std::string prompt = (mState == GameState::GameOver) ? "  > " : ("  " + GameView::sideName(mEngine.getCurrentSide()) + " > ");

	std::string		  line;
	if (!readLine(prompt.c_str(), line))
	{
		mRunning = false;
		return;
	}

	dispatch(CommandParser::parse(line));
}


void ConsoleApp::dispatch(const Command &command)
{
	switch (command.type)
	{
	case CommandType::None: break;

	case CommandType::Invalid: mView.printError(command.error); break;

	case CommandType::Quit: mRunning = false; break;

	case CommandType::Help: mView.printHelp(); break;

	case CommandType::ShowBoard: mBoardDirty = true; break;

	case CommandType::Flip:
		mView.toggleOrientation();
		mBoardDirty = true;
		break;

	case CommandType::History: mView.printHistory(mMoveLog); break;

	case CommandType::ListMoves:
		if (mState == GameState::GameOver)
			mView.printMessage("The game is over.");
		else if (command.square != Square::None)
			mView.printLegalMoves(command.square);
		else
			mView.printAllLegalMoves();
		break;

	case CommandType::Undo:
	{
		if (mMoveLog.empty())
		{
			mView.printMessage("Nothing to undo.");
			break;
		}

		if (mState == GameState::GameOver)
		{
			mView.printMessage("The game is over - type 'new' to play again.");
			break;
		}

		mEngine.undoMove();
		pumpUntilIdle(2000ms);

		// Against the computer one ply would just hand the move straight back,
		// so take back the pair.
		if (mMode == GameModeSelection::SinglePlayer && !mMoveLog.empty())
		{
			mEngine.undoMove();
			pumpUntilIdle(2000ms);
		}
		break;
	}

	case CommandType::NewGame:
		mEngine.resetGame();
		pumpUntilIdle(2000ms);

		if (!startNewGame())
			mRunning = false;
		break;

	case CommandType::Move: submitMove(command); break;
	}
}


void ConsoleApp::submitMove(const Command &command)
{
	if (mState == GameState::GameOver)
	{
		mView.printMessage("The game is over - type 'new' to play again.");
		return;
	}

	Square	  from		= command.from;
	Square	  to		= command.to;
	PieceType promotion = command.promotion;

	if (!command.SAN.empty())
	{
		if (!resolveSan(command.SAN, from, to, promotion))
		{
			mView.printError("'" + command.SAN + "' is not a legal move. Try 'moves' to see what is available.");
			return;
		}
	}
	else
	{
		MoveList legal;
		mEngine.getLegalMovesFromSquare(from, legal);

		bool matches = false;
		for (const Move &move : legal)
		{
			if (move.to() != to)
				continue;

			if (promotion == PieceType::None || move.isPromotion())
			{
				matches = true;
				break;
			}
		}

		if (!matches)
		{
			mView.printError("Illegal move. Try 'moves' to see what is available.");
			return;
		}
	}

	mEngine.submitMove(from, to, promotion);
	pumpUntilIdle(2000ms);
}


bool ConsoleApp::resolveSan(const std::string &san, Square &from, Square &to, PieceType &promotion) const
{
	const std::string target = toLowerAscii(stripCheckSuffix(san));

	for (const Move &move : mEngine.getLegalMoves())
	{
		const std::string notation = toLowerAscii(stripCheckSuffix(mEngine.getMoveNotation(move)));

		if (notation != target)
			continue;

		from	  = move.from();
		to		  = move.to();
		promotion = move.isPromotion() ? promotionFromOffset(move.promotionPieceOffset()) : PieceType::None;
		return true;
	}

	return false;
}


bool ConsoleApp::readLine(const char *prompt, std::string &line) const
{
	printf("%s", prompt);
	fflush(stdout);

	return static_cast<bool>(std::getline(std::cin, line));
}
