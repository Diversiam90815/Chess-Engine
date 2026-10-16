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

constexpr auto cpuDifficultyToString(const CPUDifficulty diff)
{
	switch (diff)
	{
	case CPUDifficulty::Easy: return "Easy";
	case CPUDifficulty::Medium: return "Medium";
	case CPUDifficulty::Hard: return "Hard";
	default: return "Unknown Difficulty";
	}
}


// <summary>
// Represents the various states of a game used in the StateMachine.
// </summary>
enum class GameState
{
	Init				 = 1,
	WaitingForInput		 = 2,
	WaitingForTarget	 = 3,
	PawnPromotion		 = 4,
	WaitingForCPUMove	 = 6,
	GameOver			 = 7,
};

constexpr auto gameStateToString(GameState state)
{
	switch (state)
	{
	case GameState::Init: return "Init";
	case GameState::WaitingForInput: return "Waiting For Input";
	case GameState::WaitingForTarget: return "Waiting For Target";
	case GameState::GameOver: return "Game Over";
	case GameState::PawnPromotion: return "Pawn Promotion";
	case GameState::WaitingForCPUMove: return "Waiting for CPU move";
	default: return "Unknown GameState";
	}
}


enum class DrawReason
{
	None				 = 0,
	FiftyMoveRule		 = 1,
	InsufficientMaterial = 2,
	ThreefoldRepetition	 = 3,
};

constexpr auto drawReasonToString(const DrawReason reason)
{
	switch (reason)
	{
	case DrawReason::FiftyMoveRule: return "Fifty-move rule";
	case DrawReason::InsufficientMaterial: return "Insufficient material";
	case DrawReason::ThreefoldRepetition: return "Threefold repetition";
	default: return "None";
	}
}

