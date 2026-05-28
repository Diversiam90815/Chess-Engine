/*
  ==============================================================================
	Module:         FileManager
	Description:    Managing application file paths (eg. logging).
  ==============================================================================
*/

#include "FileManager.h"


fs::path FileManager::getLoggingPath()
{
	std::string projectAppDataString = UserSettingsCache::GetInstance()->getAppDataPath();
	fs::path	appdataFolder		 = fs::path(projectAppDataString);
	fs::path	path				 = appdataFolder / FileName::LoggingFolder;

	createDirectoryIfNeeded(path);

	return path;
}


bool FileManager::createDirectoryIfNeeded(const fs::path &directory)
{
	std::error_code ec;

	if (!fs::exists(directory) && !fs::create_directories(directory, ec))
		return false;

	return true;
}
