/*
  ==============================================================================
	Module:         ConsoleLogging
	Description:    Writes the engine's log lines to a log file of the console app
  ==============================================================================
*/

#pragma once

#include <filesystem>


namespace ConsoleLogging
{

/**
 * @brief	Writes the engine's log lines to <folder>/Logs/Chess.log (a new file per session).
 *			An empty folder means the working directory.
 */
void enable(const std::filesystem::path &folder);

// Stops passing lines to the file. Call before the process ends.
void disable();

// Where the console app keeps its files: %LOCALAPPDATA%/ChessEngine or ~/.chessengine
std::filesystem::path defaultFolder();

} // namespace ConsoleLogging
