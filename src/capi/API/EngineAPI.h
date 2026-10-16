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

	Engine_API void	 Init();
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


#ifdef __cplusplus
}
#endif // __cplusplus
