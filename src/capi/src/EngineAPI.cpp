/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#include <strsafe.h>
#include <combaseapi.h>

#include "EngineAPI.h"

#include "GameManager.h"
#include "FileManager.h"
#include "Logging.h"
#include "UserSettingsCache.h"


//=============================================
//			API Helper Functions
//=============================================


static char *StringToCharPtr(std::string string)
{
	size_t len	   = string.length() + 1;

	char  *charPtr = (char *)CoTaskMemAlloc(len);

	if (charPtr == nullptr)
		return nullptr;


	HRESULT hr = StringCbCopy(charPtr, len, string.c_str());

	if (FAILED(hr))
	{
		CoTaskMemFree(charPtr);
		return nullptr;
	}

	return charPtr;
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

Engine_API void StartedMultiplayer()
{
	GameManager::GetInstance()->startMultiplayerSession();
}

Engine_API void StartRemoteDiscovery(bool isHost)
{
	GameManager::GetInstance()->startRemoteDiscovery(isHost);
}

Engine_API void AnswerConnectionInvitation(bool accept)
{
	GameManager::GetInstance()->answerConnectionInvitation(accept);
}

Engine_API void SendConnectionRequestToHost()
{
	GameManager::GetInstance()->sendConnectionRequestToHost();
}

Engine_API void StoppedMultiplayer()
{
	GameManager::GetInstance()->stoppedMultiplayer();
}

Engine_API void SetLocalPlayer(int iLocalPlayer)
{
	Side local = (Side)iLocalPlayer;
	GameManager::GetInstance()->setLocalPlayerInMultiplayer(local);
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
// Network
//=========================================================================

Engine_API int GetNetworkAdapterCount()
{
	auto   adapters	   = GameManager::GetInstance()->getNetworkAdapters();
	size_t numAdapters = adapters.size();
	return numAdapters;
}

Engine_API bool GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter)
{
	auto adapters = GameManager::GetInstance()->getNetworkAdapters();

	if (index >= adapters.size())
		return false;

	const auto &foundAdapter = adapters[index];

	adapter->ID				 = foundAdapter.ID;
	adapter->type			 = static_cast<int>(foundAdapter.Type);
	adapter->visibility		 = static_cast<int>(foundAdapter.Visibility);
	StringCbCopyA(adapter->adapterName, MAX_STRING_LENGTH, foundAdapter.AdapterName.c_str());
	StringCbCopyA(adapter->networkName, MAX_STRING_LENGTH, foundAdapter.NetworkName.c_str());
	return true;
}

Engine_API bool ChangeCurrentAdapter(int ID)
{
	bool result = GameManager::GetInstance()->changeCurrentNetworkAdapter(ID);
	return result;
}


Engine_API int GetCurrentNetworkAdapterID()
{
	// TODO:
	return 0;
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
