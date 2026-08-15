/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#include "EngineAPI.h"

#include "EngineInstance.h"
#include "Logging.h"
#include "TypeConversion.h"


//=============================================
//			Core Engine Lifecycle
//=============================================


Engine_API void Init(CUserSettingsInit settings)
{
	EngineInstance::Startup(ToEngineSettings(settings));
}


Engine_API void Deinit()
{
	EngineInstance::Shutdown();
}


Engine_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	if (auto *bridge = EngineInstance::Bridge())
		bridge->setDelegate(pDelegate);
}


//=============================================
//			Game Management
//=============================================

Engine_API void StartGame(CGameConfiguration config)
{
	if (auto *engine = EngineInstance::Engine())
		engine->startGame(ToGameConfiguration(config));
}


Engine_API void ResetGame()
{
	if (auto *engine = EngineInstance::Engine())
		engine->resetGame();
}


Engine_API void UndoMove()
{
	if (auto *engine = EngineInstance::Engine())
		engine->undoMove();
}


Engine_API bool GetBoardState(int *boardState)
{
	if (!boardState)
		return false;

	auto *engine = EngineInstance::Engine();
	if (!engine)
		return false;

	std::array<PieceType, 64> pieces = engine->getBoardPieces();

	for (int sq = 0; sq < 64; ++sq)
	{
		boardState[sq] = static_cast<int>(pieces[sq]);
	}

	return true;
}


Engine_API int GetGameState()
{
	auto *engine = EngineInstance::Engine();
	return engine ? static_cast<int>(engine->getGameState()) : 0;
}


Engine_API int GetCurrentSide()
{
	auto *engine = EngineInstance::Engine();
	return engine ? static_cast<int>(engine->getCurrentSide()) : static_cast<int>(Side::None);
}


Engine_API bool IsInCheck()
{
	auto *engine = EngineInstance::Engine();
	return engine ? engine->isInCheck() : false;
}


//=============================================
//			Move Input & Generation
//=============================================

Engine_API void OnSquareSelected(int square)
{
	if (square < 0 || square >= 64)
		return;

	if (auto *engine = EngineInstance::Engine())
		engine->onSquareSelected(static_cast<Square>(square));
}


Engine_API void OnPawnPromotionChosen(int pieceType)
{
	if (auto *engine = EngineInstance::Engine())
		engine->onPromotionChosen(static_cast<PieceType>(pieceType));
}


Engine_API void SubmitMove(int from, int to, int promotion)
{
	if (from < 0 || from >= 64 || to < 0 || to >= 64)
		return;

	if (auto *engine = EngineInstance::Engine())
		engine->submitMove(static_cast<Square>(from), static_cast<Square>(to), static_cast<PieceType>(promotion));
}


Engine_API int GetNumLegalMoves()
{
	auto *engine = EngineInstance::Engine();
	if (!engine)
		return 0;

	return static_cast<int>(engine->getSelectionMoves().size());
}


Engine_API bool GetLegalMoveAtIndex(int index, MoveInstance *move)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !move)
		return false;

	const MoveList &moves = engine->getSelectionMoves();

	if (index < 0 || index >= static_cast<int>(moves.size()))
		return false;

	move->data = moves[index].raw();
	return true;
}


Engine_API int GetLegalMovesForSquare(int square, MoveInstance *moves, int maxMoves)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !moves || maxMoves <= 0)
		return -1;

	if (square < 0 || square >= 64)
		return -1;

	MoveList found;
	engine->getLegalMovesFromSquare(static_cast<Square>(square), found);

	int count = static_cast<int>(found.size());
	if (count > maxMoves)
		count = maxMoves;

	for (int i = 0; i < count; ++i)
	{
		moves[i].data = found[i].raw();
	}

	return count;
}


//=========================================================================
// Multiplayer
//=========================================================================

Engine_API void StartMultiplayer()
{
	if (auto *engine = EngineInstance::Engine())
		engine->startMultiplayer();
}


Engine_API void StopMultiplayer()
{
	if (auto *engine = EngineInstance::Engine())
		engine->stopMultiplayer();
}


Engine_API void FindOpponent()
{
	if (auto *engine = EngineInstance::Engine())
		engine->findOpponent();
}


Engine_API int GetDiscoveredOpponentCount()
{
	auto *engine = EngineInstance::Engine();
	if (!engine)
		return 0;

	return static_cast<int>(engine->getDiscoveredOpponents().size());
}


Engine_API bool GetDiscoveredOpponentAtIndex(int index, char *name, int maxLen)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !name || maxLen <= 0)
		return false;

	auto opponents = engine->getDiscoveredOpponents();

	if (index < 0 || index >= static_cast<int>(opponents.size()))
		return false;

	CopyStringToBuffer(name, static_cast<size_t>(maxLen), opponents[index]);
	return true;
}


Engine_API void ConnectToOpponent(int index)
{
	if (auto *engine = EngineInstance::Engine())
		engine->connectToOpponent(index);
}


Engine_API void RespondToConnectionRequest(bool accept)
{
	if (auto *engine = EngineInstance::Engine())
		engine->respondToConnectionRequest(accept);
}


Engine_API void SetLocalPlayer(int iLocalPlayer)
{
	if (auto *engine = EngineInstance::Engine())
		engine->setLocalPlayerColor(static_cast<Side>(iLocalPlayer));
}


Engine_API void SetLocalPlayerReady(bool ready)
{
	if (auto *engine = EngineInstance::Engine())
		engine->setLocalPlayerReady(ready);
}


//=========================================================================
// Logging
//=========================================================================

Engine_API void LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Info, className, lineNumber, method, message);
}

Engine_API void LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Error, className, lineNumber, method, message);
}

Engine_API void LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Warn, className, lineNumber, method, message);
}

Engine_API void LogDebugWithCaller(const char *message, const char *method, const char *className, const int lineNumber)
{
	logging::log(LogLevel::Debug, className, lineNumber, method, message);
}


//=========================================================================
// Network Adapters
//=========================================================================

Engine_API int GetNetworkAdapterCount()
{
	auto *engine = EngineInstance::Engine();
	if (!engine)
		return 0;

	return static_cast<int>(engine->getNetworkAdapters().size());
}

Engine_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !adapter)
		return false;

	auto adapters = engine->getNetworkAdapters();

	if (index >= adapters.size())
		return false;

	const auto &found = adapters[index];

	adapter->ID		  = found.id;
	adapter->priority = static_cast<int>(found.priority);
	CopyStringToBuffer(adapter->adapterName, MAX_STRING_LENGTH, found.adapterName);
	CopyStringToBuffer(adapter->networkName, MAX_STRING_LENGTH, found.networkName);
	return true;
}

Engine_API bool ChangeCurrentAdapter(int ID)
{
	auto *engine = EngineInstance::Engine();
	return engine ? engine->changeCurrentNetworkAdapter(ID) : false;
}

Engine_API int GetCurrentNetworkAdapterID()
{
	auto *engine = EngineInstance::Engine();
	return engine ? engine->getCurrentNetworkAdapterID() : 0;
}


//=========================================================================
// Settings
//=========================================================================

Engine_API void SetLocalPlayerName(const char *name)
{
	if (auto *engine = EngineInstance::Engine())
		engine->setLocalPlayerName(name ? name : "");
}


Engine_API void SetDiscoveryPort(int port)
{
	if (auto *engine = EngineInstance::Engine())
		engine->setDiscoveryPort(port);
}
