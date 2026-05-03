/*
  ==============================================================================
	Module:         FileManager
	Description:    Managing application file paths (eg. logging).
  ==============================================================================
*/

#pragma once

#include <filesystem>

#include "Parameters.h"
#include "UserSettingsCache.h"


namespace fs = std::filesystem;

/**
 * @brief	Manages application file paths.
 */
class FileManager
{
public:
	FileManager()  = default;
	~FileManager() = default;

	fs::path getLoggingPath();

private:
	bool createDirectoryIfNeeded(const fs::path &directory);
};
