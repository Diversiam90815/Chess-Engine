/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#pragma once

#include "EngineTypes.h"


#if defined(_WIN32)
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __attribute__((visibility("default")))
#endif


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	//=========================================================================
	// Core Engine Lifecycle
	//=========================================================================

	Engine_API void	 Init(CUserSettingsInit settings);
	Engine_API void	 Deinit();
	Engine_API void	 SetDelegate(PFN_CALLBACK pDelegate);


	//=========================================================================
	// Game Management
	//=========================================================================

	Engine_API void	 StartGame(CGameConfiguration config);
	Engine_API void	 ResetGame();
	Engine_API void	 UndoMove();
	Engine_API bool	 GetBoardState(int *boardstate);

	// Current GameState value, or 0 if the engine is not running.
	Engine_API int	 GetGameState();

	// Current Side value (0 = White, 1 = Black, -1 = None).
	Engine_API int	 GetCurrentSide();

	// True if the side to move is in check.
	Engine_API bool	 IsInCheck();


	//=========================================================================
	// Move Input & Generation
	//=========================================================================

	Engine_API void	 OnSquareSelected(int square);
	Engine_API void	 OnPawnPromotionChosen(int pieceType);

	Engine_API void	 SubmitMove(int from, int to, int promotion);

	Engine_API int	 GetNumLegalMoves();
	Engine_API bool	 GetLegalMoveAtIndex(int index, MoveInstance *move);
	Engine_API int	 GetLegalMovesForSquare(int square, MoveInstance *moves, int maxMoves);


	//=========================================================================
	// Multiplayer
	//=========================================================================

	Engine_API void	 StartMultiplayer();
	Engine_API void	 StopMultiplayer();

	Engine_API void	 FindOpponent();
	Engine_API int	 GetDiscoveredOpponentCount();
	Engine_API bool	 GetDiscoveredOpponentAtIndex(int index, char *name, int maxLen);
	Engine_API void	 ConnectToOpponent(int index);
	Engine_API void	 RespondToConnectionRequest(bool accept);

	Engine_API void	 SetLocalPlayer(int iLocalPlayer);
	Engine_API void	 SetLocalPlayerReady(bool ready);


	//=========================================================================
	// Logging
	//=========================================================================

	Engine_API void	 LogInfoWithCaller(const char *message, const char *method, const char *className, const int lineNumber);
	Engine_API void	 LogErrorWithCaller(const char *message, const char *method, const char *className, const int lineNumber);
	Engine_API void	 LogWarningWithCaller(const char *message, const char *method, const char *className, const int lineNumber);
	Engine_API void	 LogDebugWithCaller(const char *message, const char *method, const char *className, const int lineNumber);


	//=========================================================================
	// Network Adapters
	//=========================================================================

	Engine_API int	 GetNetworkAdapterCount();
	Engine_API bool	 GetNetworkAdapterAtIndex(unsigned int index, NetworkAdapterInstance *adapter);
	Engine_API bool	 ChangeCurrentAdapter(int ID);
	Engine_API int	 GetCurrentNetworkAdapterID();


	//=========================================================================
	// Settings
	//=========================================================================

	Engine_API void	 SetLocalPlayerName(const char *name);
	Engine_API void	 SetDiscoveryPort(int port);


#ifdef __cplusplus
}
#endif // __cplusplus
