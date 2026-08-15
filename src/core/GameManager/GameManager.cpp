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


bool GameManager::init(EngineSettings settings)
{
	if (mInitialized)
		return true;

	mSettings = std::move(settings);

	mLog.initLogging(mSettings.logFolder);

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

	if (mMultiplayerManager)
		mMultiplayerManager->shutdown();

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


//=========================================================================
// Multiplayer
//=========================================================================

void GameManager::ensureMultiplayerManager()
{
	if (mMultiplayerManager)
		return;

	mMultiplayerManager = std::make_unique<MultiplayerManager>();
	mMultiplayerManager->setEventQueue(&mEvents);
	mMultiplayerManager->setRemoteMoveCallback([this](Move move) { mStateMachine->onRemoteMoveReceived(move); });
}


void GameManager::startMultiplayer()
{
	LOG_INFO("Multiplayer started");

	ensureMultiplayerManager();
	mMultiplayerManager->init(mSettings.playerName, mSettings.discoveryPort);

	mIsMultiplayerMode = true;
}


void GameManager::stopMultiplayer()
{
	if (mMultiplayerManager)
		mMultiplayerManager->disconnect();

	mIsMultiplayerMode = false;
	resetGame();
}


void GameManager::findOpponent()
{
	if (!mMultiplayerManager)
		return;

	mMultiplayerManager->findOpponent();
}


std::vector<std::string> GameManager::getDiscoveredOpponents()
{
	if (!mMultiplayerManager)
		return {};

	return mMultiplayerManager->getDiscoveredOpponents();
}


void GameManager::connectToOpponent(int index)
{
	if (!mMultiplayerManager)
		return;

	mMultiplayerManager->connectToOpponent(index);
}


void GameManager::respondToConnectionRequest(bool accept)
{
	if (!mMultiplayerManager)
		return;

	mMultiplayerManager->respondToConnectionRequest(accept);
}


void GameManager::setLocalPlayerColor(Side localPlayer)
{
	if (!mMultiplayerManager)
		return;

	mMultiplayerManager->sendPlayerChosen(localPlayer);
}


void GameManager::setLocalPlayerReady(bool ready)
{
	if (!mMultiplayerManager)
		return;

	mMultiplayerManager->sendPlayerReady(ready);
}


std::vector<netlink::NetworkAdapter> GameManager::getNetworkAdapters()
{
	ensureMultiplayerManager();
	return mMultiplayerManager->getAvailableAdapters();
}


bool GameManager::changeCurrentNetworkAdapter(int ID)
{
	ensureMultiplayerManager();
	return mMultiplayerManager->setActiveAdapter(ID);
}


int GameManager::getCurrentNetworkAdapterID()
{
	ensureMultiplayerManager();
	return mMultiplayerManager->getActiveAdapterID();
}
