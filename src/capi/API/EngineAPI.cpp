/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#include <cstring>

#include "EngineAPI.h"

#include "GameManager.h"
#include "FileManager.h"
#include "Logging.h"
#include "UserSettingsCache.h"


//=============================================
//			API Helper Functions
//=============================================


static void copyStringToBuffer(char *dest, size_t destSize, const std::string &src)
{
	if (!dest || destSize == 0)
		return;

	size_t len = src.size() < destSize - 1 ? src.size() : destSize - 1;

	std::memcpy(dest, src.data(), len);
	dest[len] = '\0';
}


static GameConfiguration ConvertCConfig(CGameConfiguration cConfig)
{
	GameModeSelection mode		 = static_cast<GameModeSelection>(cConfig.mode);
	Side			  playerSide = static_cast<Side>(cConfig.playerColor);
	CPUDifficulty	  difficulty = static_cast<CPUDifficulty>(cConfig.cpuDifficulty);

	switch (mode)
	{
	case GameModeSelection::LocalCoop: return GameConfiguration::createLocalCoop();
	case GameModeSelection::SinglePlayer: return GameConfiguration::createSinglePlayer(playerSide, difficulty);
	case GameModeSelection::Multiplayer: return GameConfiguration::createMultiplayer(playerSide);
	default: LOG_ERROR("Invalid game mode received from C API: {}", cConfig.mode); break;
	}

	// fallback: local coop
	return GameConfiguration::createLocalCoop();
}


static UserSettingsInit ConvertCSettings(CUserSettingsInit cSettings)
{
	UserSettingsInit init;
	init.playerName		  = cSettings.playerName;
	init.discoveryUDPPort = cSettings.discoveryUDPPort;
	init.appDataPath	  = cSettings.appDataPath;
	return init;
}



//=============================================
//			Core Engine Lifecycle
//=============================================


Engine_API void Init(CUserSettingsInit settings)
{
	UserSettingsInit init = ConvertCSettings(settings);

	UserSettingsCache::GetInstance()->initialize(init);
	GameManager::GetInstance()->init();
}


Engine_API void Deinit()
{
	GameManager::ReleaseInstance();
	UserSettingsCache::ReleaseInstance();
}


Engine_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	GameManager::GetInstance()->setDelegate(pDelegate);
}


//=============================================
//			Game Management
//=============================================

Engine_API void StartGame(CGameConfiguration config)
{
	GameConfiguration cppConfig = ConvertCConfig(config);
	GameManager::GetInstance()->startGame(cppConfig);
}


Engine_API void ResetGame()
{
	GameManager::GetInstance()->resetGame();
}


Engine_API void UndoMove()
{
	GameManager::GetInstance()->undoMove();
}


Engine_API bool GetBoardState(int *boardState)
{
	if (!boardState)
		return false;

	GameManager *manager = GameManager::GetInstance();
	if (!manager)
		return false;

	std::array<PieceType, 64> pieces = manager->getBoardPieces();

	for (int sq = 0; sq < 64; ++sq)
	{
		boardState[sq] = static_cast<int>(pieces[sq]);
	}

	return true;
}


//=============================================
//			Move Input & Generation
//=============================================

Engine_API void OnSquareSelected(int square)
{
	if (square < 0 || square >= 64)
		return;

	Square sq = static_cast<Square>(square);
	GameManager::GetInstance()->onSquareSelected(sq);
}


Engine_API void OnPawnPromotionChosen(int pieceType)
{
	PieceType promotion = static_cast<PieceType>(pieceType);
	GameManager::GetInstance()->onPromotionChosen(promotion);
}


Engine_API int GetNumLegalMoves()
{
	const MoveList &moves = GameManager::GetInstance()->getCachedLegalMoves();
	return static_cast<int>(moves.size());
}


Engine_API bool GetLegalMoveAtIndex(int index, MoveInstance *move)
{
	const MoveList &moves = GameManager::GetInstance()->getCachedLegalMoves();

	if (index < 0 || index >= static_cast<int>(moves.size()))
		return false;

	move->data = moves[index].raw();
	return true;
}


//=========================================================================
// Multiplayer
//=========================================================================

Engine_API void StartMultiplayer()
{
	GameManager::GetInstance()->startMultiplayer();
}


Engine_API void StopMultiplayer()
{
	GameManager::GetInstance()->stopMultiplayer();
}


Engine_API void FindOpponent()
{
	GameManager::GetInstance()->findOpponent();
}


Engine_API int GetDiscoveredOpponentCount()
{
	auto opponents = GameManager::GetInstance()->getDiscoveredOpponents();
	return static_cast<int>(opponents.size());
}


Engine_API bool GetDiscoveredOpponentAtIndex(int index, char *name, int maxLen)
{
	auto opponents = GameManager::GetInstance()->getDiscoveredOpponents();

	if (index < 0 || index >= static_cast<int>(opponents.size()))
		return false;

	if (!name || maxLen <= 0)
		return false;

	copyStringToBuffer(name, static_cast<size_t>(maxLen), opponents[index]);
	return true;
}


Engine_API void ConnectToOpponent(int index)
{
	GameManager::GetInstance()->connectToOpponent(index);
}


Engine_API void RespondToConnectionRequest(bool accept)
{
	GameManager::GetInstance()->respondToConnectionRequest(accept);
}


Engine_API void SetLocalPlayer(int iLocalPlayer)
{
	Side local = static_cast<Side>(iLocalPlayer);
	GameManager::GetInstance()->setLocalPlayerColor(local);
}


Engine_API void SetLocalPlayerReady(bool ready)
{
	GameManager::GetInstance()->setLocalPlayerReady(ready);
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
	auto adapters = GameManager::GetInstance()->getNetworkAdapters();
	return static_cast<int>(adapters.size());
}

Engine_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
{
	auto adapters = GameManager::GetInstance()->getNetworkAdapters();

	if (index >= adapters.size())
		return false;

	const auto &found = adapters[index];

	adapter->ID		  = found.id;
	adapter->priority = static_cast<int>(found.priority);
	copyStringToBuffer(adapter->adapterName, MAX_STRING_LENGTH, found.adapterName);
	copyStringToBuffer(adapter->networkName, MAX_STRING_LENGTH, found.networkName);
	return true;
}

Engine_API bool ChangeCurrentAdapter(int ID)
{
	return GameManager::GetInstance()->changeCurrentNetworkAdapter(ID);
}

Engine_API int GetCurrentNetworkAdapterID()
{
	return GameManager::GetInstance()->getCurrentNetworkAdapterID();
}


//=========================================================================
// Settings
//=========================================================================

Engine_API void SetLocalPlayerName(const char *name)
{
	UserSettingsCache::GetInstance()->setLocalPlayerName(name ? name : "");
}


Engine_API void SetDiscoveryPort(int port)
{
	UserSettingsCache::GetInstance()->setDiscoveryPort(port);
}


//=========================================================================
// Utilities
//=========================================================================

Engine_API float GetWindowScalingFactor(HWND hwnd)
{
	int	  dpi			= GetDpiForWindow(hwnd);
	float scalingFactor = (float)dpi / 96;
	return scalingFactor;
}
