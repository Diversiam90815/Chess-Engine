/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/



#pragma once

#include <Logger.h>
#include <string>

#include "ConnectionStatus.h"
#include "FileManager.h"


/// <summary>
/// Provides logging functionality, including initialization of the logging system.
/// </summary>
class Logging
{
public:
	Logging()  = default;
	~Logging() = default;

	void			   initLogging();

	static std::string gameStateToString(GameState state);
	static std::string boolToString(const bool value);
	static std::string connectionStateToString(const ConnectionState state);
	static std::string sideToString(const Side side);
	static std::string cpuDifficultyToString(const CPUDifficulty diff);

private:
	FileManager fmg;

	std::string mLoggerName;
	const int	mSlowLogTimeMS = 500;
};
