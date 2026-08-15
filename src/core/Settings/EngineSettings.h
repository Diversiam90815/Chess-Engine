/*
  ==============================================================================
	Module:         EngineSettings
	Description:    Host-supplied configuration handed to the engine at startup
  ==============================================================================
*/

#pragma once

#include <filesystem>
#include <string>


/**
 * @brief	Everything the engine needs from its host at initialization.
 *			Passed by value into GameManager::init() and owned by it from then on.
 */
struct EngineSettings
{
	// Display name used for multiplayer discovery.
	std::string			  playerName;

	// Root folder for engine-written files (logs). Empty = current working directory.
	std::filesystem::path logFolder;

	// UDP port used for LAN opponent discovery.
	int					  discoveryPort = 0;
};
