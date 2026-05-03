/*
  ==============================================================================
	Module:         UserSettingsCache
	Description:    In-memory cache for user settings, populated by the C# host.
  ==============================================================================
*/

#pragma once

#include <string>
#include <mutex>


/// <summary>
/// Singleton cache holding user settings.
/// </summary>
class UserSettingsCache
{
public:
	static UserSettingsCache *GetInstance();
	static void				  ReleaseInstance();


	// Non-copyable / non-movable
	UserSettingsCache(const UserSettingsCache &)			= delete;
	UserSettingsCache &operator=(const UserSettingsCache &) = delete;

	//=========================================================================
	// Player
	//=========================================================================

	void			   setLocalPlayerName(const std::string &name);
	std::string		   getLocalPlayerName() const;


	//=========================================================================
	// Discovery
	//=========================================================================

	void			   setDiscoveryPort(const int port);
	int				   getDiscoveryPort() const;


private:
	UserSettingsCache() = default;

	mutable std::mutex mMutex;

	// Player
	std::string		   mLocalPlayerName = "";

	// Discovery
	int				   mDiscoveryPort	= 0;
};