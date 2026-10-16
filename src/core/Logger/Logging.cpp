/*
  ==============================================================================
	Module:         Logger
	Description:    Text of engine values for log lines
  ==============================================================================
*/


#include "Logging.h"


std::string Logging::gameStateToString(GameState state)
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


std::string Logging::boolToString(const bool value)
{
	return value ? "True" : "False";
}


std::string Logging::sideToString(const Side side)
{
	switch (side)
	{
	case Side::None: return "None";
	case Side::White: return "White";
	case Side::Black: return "Black";
	case Side::Both: return "Both";
	default: return "Unknown Side";
	}
}


std::string Logging::cpuDifficultyToString(const CPUDifficulty diff)
{
	switch (diff)
	{
	case CPUDifficulty::Easy: return "Easy";
	case CPUDifficulty::Medium: return "Medium";
	case CPUDifficulty::Hard: return "Hard";
	default: return "Unknown Difficulty";
	}
}


std::string Logging::drawReasonToString(const DrawReason reason)
{
	switch (reason)
	{
	case DrawReason::FiftyMoveRule: return "Fifty-move rule";
	case DrawReason::InsufficientMaterial: return "Insufficient material";
	case DrawReason::ThreefoldRepetition: return "Threefold repetition";
	default: return "None";
	}
}
