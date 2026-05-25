/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#include "GameManager.h"
#include "UserSettingsCache.h"


GameManager::~GameManager()
{
	shutDown();
}


GameManager *GameManager::GetInstance()
{
	static GameManager *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new GameManager();
	}
	return sInstance;
}


void GameManager::ReleaseInstance()
{
	GameManager *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


bool GameManager::init()
{
	if (mInitialized)
		return true;

	mLog.initLogging();

	SystemInfo::logSystemInfo();

	initializeComponents();
	wireComponents();

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

	mInitialized = false;
}


void GameManager::startGame(GameConfiguration config)
{
	LOG_INFO("Starting game. Mode : {}", static_cast<int>(config.mode));
	mStateMachine->onGameStart(config);
}


void GameManager::setDelegate(PFN_CALLBACK pDelegate)
{
	mInputSource->setDelegate(pDelegate);
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


const MoveList &GameManager::getCachedLegalMoves() const
{
	return mGameController->getCachedLegalMoves();
}


PieceType GameManager::getPieceAt(Square sq) const
{
	return mGameController->getBoard().pieceAt(sq);
}


void GameManager::undoMove()
{
	mStateMachine->onUndoRequested();
}


void GameManager::onSquareSelected(Square sq)
{
	LOG_INFO("Square {} selected", to_index(sq));
	LOG_INFO("Square {} selected", square_to_coordinates[to_index(sq)]);

	mStateMachine->onSquareSelected(sq);
}


void GameManager::onPromotionChosen(PieceType piece)
{
	mStateMachine->onPromotionChosen(piece);
}


void GameManager::resetGame()
{
	mStateMachine->onGameReset();
}


//=========================================================================
// Multiplayer
//=========================================================================

void GameManager::startMultiplayer()
{
	LOG_INFO("Multiplayer started");

	mMultiplayerManager->init();
	mMultiplayerManager->setRemoteMoveCallback([this](Move move) { mStateMachine->onRemoteMoveReceived(move); });

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
	if (!mMultiplayerManager)
		return {};

	return mMultiplayerManager->getAvailableAdapters();
}


bool GameManager::changeCurrentNetworkAdapter(int ID)
{
	if (!mMultiplayerManager)
		return false;

	return mMultiplayerManager->setActiveAdapter(ID);
}


//=========================================================================
// Initialization
//=========================================================================

void GameManager::initializeComponents()
{
	mInputSource		= std::make_shared<WinUIInputSource>();
	mGameController		= std::make_unique<GameController>();
	mStateMachine		= std::make_unique<StateMachine>();
	mMultiplayerManager = std::make_shared<MultiplayerManager>();
}


void GameManager::wireComponents()
{
	mStateMachine->setGameController(mGameController.get());
	mStateMachine->setInputSource(mInputSource.get());

	auto &whitePlayer = mGameController->getWhitePlayer();
	whitePlayer.attachObserver(mInputSource);

	auto &blackPlayer = mGameController->getBlackPlayer();
	blackPlayer.attachObserver(mInputSource);

	mGameController->setCPUMoveCallback([this](Move move) { mStateMachine->onCPUMoveCalculated(move); });

	mMultiplayerManager->attachObserver(mInputSource);

	mStateMachine->start();
}
