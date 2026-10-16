/*
  ==============================================================================
	Module:         Logger
	Description:    Text of engine values for log lines
  ==============================================================================
*/



#pragma once

#include <string>

#include "EngineLog.h"
#include "Parameters.h"


class Logging
{
public:
	static std::string gameStateToString(GameState state);
	static std::string boolToString(const bool value);
	static std::string sideToString(const Side side);
	static std::string cpuDifficultyToString(const CPUDifficulty diff);
	static std::string drawReasonToString(const DrawReason reason);
};
