/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once

#include <array>

#include "BitboardTypes.h"


// <summary>
// Represents the possible end states of a chess game.
// </summary>
enum class EndGameState
{
	OnGoing	  = 1,
	Checkmate = 2,
	StaleMate = 3,
	Draw	  = 4,
	Reset	  = 5
};


/**
 * @brief	Representation of CPU difficulty levels
 */
enum class CPUDifficulty
{
	Easy   = 1,
	Medium = 2,
	Hard   = 3
};


// <summary>
// Represents the various states of a game used in the StateMachine.
// </summary>
enum class GameState
{
	Init				 = 1,
	WaitingForInput		 = 2,
	WaitingForTarget	 = 3,
	PawnPromotion		 = 4,
	WaitingForRemoteMove = 5,
	WaitingForCPUMove	 = 6,
	GameOver			 = 7,
};


//============================================================
//			File Manager
//============================================================

namespace FileName
{
constexpr auto LoggingFolder = "Logs";
constexpr auto LogFile		 = "Chess.log";
} // namespace FileName
