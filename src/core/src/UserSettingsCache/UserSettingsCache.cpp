/*
  ==============================================================================
	Module:         UserSettingsCache
	Description:    In-memory cache for user settings, populated by the C# host.
  ==============================================================================
*/


#include "UserSettingsCache.h"


UserSettingsCache *UserSettingsCache::GetInstance()
{
	static UserSettingsCache *sInstance = nullptr;
	if (nullptr == sInstance)
	{
		sInstance = new UserSettingsCache();
	}
	return sInstance;
}


void UserSettingsCache::ReleaseInstance()
{
	UserSettingsCache *sInstance = GetInstance();
	if (sInstance)
	{
		delete sInstance;
	}
}


void UserSettingsCache::initialize(const UserSettingsInit &init)
{
	std::lock_guard lock(mMutex);
	mLocalPlayerName = init.playerName;
	mDiscoveryPort	 = init.discoveryUDPPort;
	mAppDataPath	 = init.appDataPath;
}


void UserSettingsCache::setLocalPlayerName(const std::string &name)
{
	std::lock_guard lock(mMutex);
	mLocalPlayerName = name;
}


std::string UserSettingsCache::getLocalPlayerName() const
{
	std::lock_guard lock(mMutex);
	return mLocalPlayerName;
}


void UserSettingsCache::setDiscoveryPort(const int port)
{
	std::lock_guard lock(mMutex);
	mDiscoveryPort = port;
}


int UserSettingsCache::getDiscoveryPort() const
{
	std::lock_guard lock(mMutex);
	return mDiscoveryPort;
}
