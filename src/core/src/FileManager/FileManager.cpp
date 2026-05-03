/*
  ==============================================================================
	Module:         FileManager
	Description:    Singleton managing application file paths (logging, AppData).
  ==============================================================================
*/

#include "FileManager.h"
#include <iostream>


FileManager *FileManager::GetInstance()
{
	static FileManager *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new FileManager();
	}
	return sInstance;
}


void FileManager::ReleaseInstance()
{
	FileManager *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


fs::path FileManager::getAppDataPath()
{
	return fs::path(mAppDataPath);
}


void FileManager::setAppDataPath(std::string path)
{
	mAppDataPath = path;
}


fs::path FileManager::getLoggingPath()
{
	fs::path path = getAppDataPath() / FileName::LoggingFolder;
	createDirectoryIfNeeded(path);
	return path;
}


bool FileManager::createDirectoryIfNeeded(const fs::path &directory)
{
	std::error_code ec;
	if (!fs::exists(directory) && !fs::create_directories(directory, ec))
	{
		std::cerr << "Failed to create directory!" << std::endl;
		return false;
	}
	return true;
}
