/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/



#pragma once

#include <string>
#include <filesystem>

#include "Parameters.h"
#include "Logger.h"
#include "MultiplayerTypes.h"

namespace fs = std::filesystem;


class Logging
{
public:
	Logging()  = default;
	~Logging() = default;

	void			   initLogging(const fs::path &loggingFolder);

	static std::string gameStateToString(GameState state);
	static std::string boolToString(const bool value);
	static std::string multiplayerStateToString(MultiplayerState state);
	static std::string sideToString(const Side side);
	static std::string cpuDifficultyToString(const CPUDifficulty diff);
	static std::string drawReasonToString(const DrawReason reason);

private:
	std::string getNewLogFilePath(fs::path logFolder);

	std::string mLoggerName;
	const int	mSlowLogTimeMS = 500;
};
