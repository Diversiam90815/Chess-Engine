/*
  ==============================================================================
	Module:         EngineTypes
	Description:    Plain C types shared across the Chess engine's C ABI
  ==============================================================================
*/

#pragma once

#include <cstdint>

#define BOARD_SIZE		  8
#define MAX_STRING_LENGTH 250


#ifdef __cplusplus
extern "C"
{
#endif

	//=========================================================================
	// Callback Delegates
	//=========================================================================

	// Same calling convention as the Win32 CALLBACK macro, without dragging
	// windows.h into a header that managed hosts also consume.
#if defined(_WIN32)
#define API_CALLBACK __stdcall
#else
#define API_CALLBACK
#endif

	typedef void(API_CALLBACK *PFN_CALLBACK)(int messageId, void *pContext);

	/**
	 * @brief	Receives the engine's log lines (see SetLogCallback).
	 *			level: 0 Trace, 1 Debug, 2 Info, 3 Warning, 4 Error, 5 Critical. Texts are UTF-8 and only
	 *			valid during the call. Called on whatever engine thread logs; it must not call back into the engine.
	 */
	typedef void(API_CALLBACK *PFN_LOG_CALLBACK)(int level, const char *file, int line, const char *function, const char *message, void *context);


	//=========================================================================
	// Structures
	//=========================================================================

	/**
	 * @brief	Compact move representation (16-bit encoded)
	 *			Matches C++ Move class: from (6 bits) | to (6 bits) | flags (4 bits)
	 */
	typedef struct
	{
		uint16_t data;
	} MoveInstance;


	/**
	 * @brief	Extended move information for UI
	 */
	typedef struct
	{
		int	 from;			// Square index 0-63
		int	 to;			// Square index 0-63
		int	 flags;			// MoveFlag value
		int	 movedPiece;	// PieceType value
		int	 capturedPiece; // PieceType value or -1
		char notation[16];	// SAN notation string
	} MoveInfoInstance;


	/**
	 * @brief	Game Phases for UI
	 */
	enum UIGamePhase
	{
		Initializing	= 0,
		PlayerTurn		= 1,
		OpponentTurn	= 2,
		PromotionDialog = 3,
		GameEnded		= 4,
	};


	/**
	 * @brief	Game configuration for C API
	 *			Simplified flat structure for C#/C interop
	 */
	typedef struct
	{
		int mode;		   // GameModeSelection: 1=LocalCoop, 2=SinglePlayer
		int playerColor;   // Side: 0=White, 1=Black (the human player in SinglePlayer)
		int cpuDifficulty; // CPUDifficulty: 1=Easy, 2=Medium, 3=Hard (only for SinglePlayer)
	} CGameConfiguration;


#ifdef __cplusplus
}
#endif
