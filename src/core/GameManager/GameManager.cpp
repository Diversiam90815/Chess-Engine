/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#include "GameManager.h"


GameManager::~GameManager()
{
	shutDown();
}


bool GameManager::init()
{
	if (mInitialized)
		return true;

	SystemInfo::logSystemInfo();

	mGameController = std::make_unique<GameController>();
	mStateMachine	= std::make_unique<StateMachine>();

	mGameController->setEventQueue(&mEvents);
	mGameController->setCPUMoveCallback([this](Move move) { mStateMachine->onCPUMoveCalculated(move); });

	mStateMachine->setGameController(mGameController.get());
	mStateMachine->setEventQueue(&mEvents);
	mStateMachine->start();

	mInitialized = true;
	LOG_INFO("GameManager initialized!");

	return true;
}


void GameManager::shutDown()
{
	if (!mInitialized)
		return;

	LOG_INFO("Shutting down Game Manager!");

	if (mStateMachine)
		mStateMachine->stop();

	if (mGameController)
		mGameController->cancelCPUCalculation();

	mEvents.close();

	mInitialized = false;
}


//=========================================================================
// Game Control
//=========================================================================

void GameManager::startGame(GameConfiguration config)
{
	LOG_INFO("Starting game. Mode : {}", static_cast<int>(config.mode));
	mStateMachine->onGameStart(config);
}


void GameManager::resetGame()
{
	mStateMachine->onGameReset();
}


void GameManager::undoMove()
{
	mStateMachine->onUndoRequested();
}


//=========================================================================
// Input Events
//=========================================================================

void GameManager::submitMove(Square from, Square to, PieceType promotion)
{
	LOG_INFO("Move submitted: {} -> {}", square_to_coordinates[to_index(from)], square_to_coordinates[to_index(to)]);

	mStateMachine->onMoveRequested(from, to, promotion);
}


void GameManager::onSquareSelected(Square sq)
{
	LOG_INFO("Square {} selected", square_to_coordinates[to_index(sq)]);

	mStateMachine->onSquareSelected(sq);
}


void GameManager::onPromotionChosen(PieceType piece)
{
	mStateMachine->onPromotionChosen(piece);
}


//=========================================================================
// Board & Game State Queries
//=========================================================================

GameState GameManager::getGameState() const
{
	return mStateMachine ? mStateMachine->getState() : GameState::Init;
}


Side GameManager::getCurrentSide() const
{
	return mGameController->getCurrentSide();
}


bool GameManager::isInCheck() const
{
	return mGameController->isInCheck();
}


std::array<PieceType, 64> GameManager::getBoardPieces() const
{
	std::array<PieceType, 64> pieces{};

	const Chessboard		 &board = mGameController->getBoard();

	for (int i = 0; i < 64; ++i)
	{
		pieces[i] = board.pieceAt(static_cast<Square>(i));
	}

	return pieces;
}


PieceType GameManager::getPieceAt(Square sq) const
{
	return mGameController->getBoard().pieceAt(sq);
}


const Chessboard &GameManager::getBoard() const
{
	return mGameController->getBoard();
}


const MoveList &GameManager::getLegalMoves() const
{
	return mGameController->getAllLegalMoves();
}


void GameManager::getLegalMovesFromSquare(Square from, MoveList &out) const
{
	out.clear();

	const MoveList &all = mGameController->getAllLegalMoves();

	for (const Move &move : all)
	{
		if (move.from() == from)
			out.push(move);
	}
}


const MoveList &GameManager::getSelectionMoves() const
{
	return mGameController->getSelectionMoves();
}


std::string GameManager::getMoveNotation(Move move) const
{
	return mGameController->getMoveNotation(move);
}


const std::vector<MoveHistoryEntry> &GameManager::getMoveHistory() const
{
	return mGameController->getMoveHistory();
}


const std::vector<PieceType> &GameManager::getCapturedPieces(Side player) const
{
	return (player == Side::White) ? mGameController->getWhitePlayer().getCapturedPieces() : mGameController->getBlackPlayer().getCapturedPieces();
}
