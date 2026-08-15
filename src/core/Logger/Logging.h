/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/



#pragma once

#include <string>

#include "Logger.h"
#include "MultiplayerTypes.h"
#include "FileManager.h"


class Logging
{
public:
	Logging()  = default;
	~Logging() = default;

	void			   initLogging();

	static std::string gameStateToString(GameState state);
	static std::string boolToString(const bool value);
	static std::string multiplayerStateToString(MultiplayerState state);
	static std::string sideToString(const Side side);
	static std::string cpuDifficultyToString(const CPUDifficulty diff);
	static std::string drawReasonToString(const DrawReason reason);

private:
	FileManager fmg;

	std::string mLoggerName;
	const int	mSlowLogTimeMS = 500;
};
