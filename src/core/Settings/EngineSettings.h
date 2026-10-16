/*
  ==============================================================================
	Module:         EngineSettings
	Description:    Host-supplied configuration handed to the engine at startup
  ==============================================================================
*/

#pragma once

#include <filesystem>


/**
 * @brief	Everything the engine needs from its host at initialization.
 *			Passed by value into GameManager::init() and owned by it from then on.
 */
struct EngineSettings
{
	// Root folder for engine-written files (logs). Empty = current working directory.
	std::filesystem::path logFolder;
};
