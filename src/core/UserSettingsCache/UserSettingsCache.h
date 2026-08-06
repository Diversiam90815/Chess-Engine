/*
  ==============================================================================
	Module:         UserSettingsCache
	Description:    In-memory cache for user settings, populated by the C# host.
  ==============================================================================
*/

#pragma once

#include <string>
#include <mutex>


/**
 * @brief	Initialization payload pushed from the C# host during startup.
 */
struct UserSettingsInit
{
	std::string playerName;
	std::string appDataPath;
	int			discoveryUDPPort = 0;
};



/**
 * @brief	Singleton cache holding user settings.
 */
class UserSettingsCache
{
public:
	static UserSettingsCache *GetInstance();
	static void				  ReleaseInstance();

	// Non-copyable / non-movable
	UserSettingsCache(const UserSettingsCache &)			= delete;
	UserSettingsCache &operator=(const UserSettingsCache &) = delete;

	void			   initialize(const UserSettingsInit &init);


	//=========================================================================
	// AppData Path
	//=========================================================================

	std::string		   getAppDataPath() const;


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

	// Cache/Logging path
	std::string		   mAppDataPath		= "";

	// Discovery
	int				   mDiscoveryPort	= 0;
};
