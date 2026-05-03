/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton managing application file paths (logging, AppData).
	                AppData path needs to be set once via the Engine API due to WinUI's virtualization.
  ==============================================================================
*/

#pragma once

#include <filesystem>

#include "Parameters.h"
#include "Logging.h"


namespace fs = std::filesystem;

/**
 * @brief	Manages application file paths. Singleton accessor pattern.
			AppData path needs to be set once via the Engine.API due to the virtualization inside WinUi apps.
 */
class FileManager
{
public:
	~FileManager() = default;

	static FileManager *GetInstance();
	static void			ReleaseInstance();

	fs::path			getAppDataPath();
	void				setAppDataPath(std::string path);
	fs::path			getLoggingPath();

private:
	FileManager() = default;

	bool		createDirectoryIfNeeded(const fs::path &directory);

	std::string mAppDataPath = "";
};
