/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#include "PlainCDefines.h"


#define Engine_API __declspec(dllexport)


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


	//=========================================================================
	// Move Input & Generation
	//=========================================================================

	Engine_API void	 OnSquareSelected(int square);
	Engine_API void	 OnPawnPromotionChosen(int pieceType);
	Engine_API int	 GetNumLegalMoves();
	Engine_API bool	 GetLegalMoveAtIndex(int index, MoveInstance *move);


	//=========================================================================
	// Multiplayer
	//=========================================================================

	Engine_API void	 StartedMultiplayer();
	Engine_API void	 StartRemoteDiscovery(bool isHost);
	Engine_API void	 AnswerConnectionInvitation(bool accept);
	Engine_API void	 SendConnectionRequestToHost();
	Engine_API void	 StoppedMultiplayer();
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
	// Network
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


	//=========================================================================
	// Utilities
	//=========================================================================

	Engine_API float GetWindowScalingFactor(HWND hwnd);


#ifdef __cplusplus
}
#endif // __cplusplus
