/*
  ==============================================================================
	Module:         MultiplayerManager
	Description:    Chess multiplayer session management using NetLink
  ==============================================================================
*/

#pragma once

#include <functional>
#include <vector>
#include <mutex>
#include <string>

#include <NetLink/NetLink.h>

#include "IObservable.h"
#include "MultiplayerTypes.h"
#include "Move.h"
#include "Logging.h"


class MultiplayerManager : public IMultiplayerObservable
{
public:
	MultiplayerManager() = default;
	~MultiplayerManager();

	MultiplayerManager(const MultiplayerManager &)							   = delete;
	MultiplayerManager					&operator=(const MultiplayerManager &) = delete;

	//=========================================================================
	// Lifecycle
	//=========================================================================

	void								 init();
	void								 shutdown();

	//=========================================================================
	// Opponent Discovery & Connection
	//=========================================================================

	void								 findOpponent();
	std::vector<std::string>			 getDiscoveredOpponents();
	void								 connectToOpponent(int index);
	void								 respondToConnectionRequest(bool accept);
	void								 disconnect();

	//=========================================================================
	// Game Setup (post-connection)
	//=========================================================================

	void								 sendPlayerChosen(Side localPlayer);
	void								 sendPlayerReady(bool ready);

	//=========================================================================
	// In-Game Messaging
	//=========================================================================

	void								 sendMove(Move move);

	//=========================================================================
	// Network Adapters
	//=========================================================================

	std::vector<netlink::NetworkAdapter> getAvailableAdapters();
	bool								 setActiveAdapter(int adapterID);

	//=========================================================================
	// Callbacks
	//=========================================================================

	void								 setRemoteMoveCallback(std::function<void(Move)> callback);

	//=========================================================================
	// State
	//=========================================================================

	MultiplayerState					 getState() const;

	//=========================================================================
	// IMultiplayerObservable
	//=========================================================================

	void								 multiplayerStateChanged(const MultiplayerEvent &event) override;
	void								 opponentFound(const std::string &name) override;
	void								 remotePlayerChosen(Side remotePlayer) override;

private:
	//=========================================================================
	// NetLink Callbacks
	//=========================================================================

	void					  onRemoteDiscovered(const netlink::Endpoint &remote);
	void					  onConnectionChanged(const netlink::ConnectionEvent event);
	void					  onMessageReceived(const netlink::Message &message);
	void					  onAdapterChanged(const netlink::NetworkAdapter &adapter);

	//=========================================================================
	// State Management
	//=========================================================================

	void					  transitionTo(MultiplayerState newState, const std::string &remoteName = "", const std::string &error = "");

	//=========================================================================
	// Message Serialization
	//=========================================================================

	static netlink::Message	  serializeMove(Move move);
	static netlink::Message	  serializePlayerChosen(Side side);
	static netlink::Message	  serializePlayerReady(bool ready);

	static Move				  deserializeMove(const netlink::Message &msg);
	static Side				  deserializePlayerChosen(const netlink::Message &msg);
	static bool				  deserializePlayerReady(const netlink::Message &msg);

	//=========================================================================
	// Members
	//=========================================================================

	netlink::NetLink		  mNetLink;
	MultiplayerState		  mState{MultiplayerState::None};
	std::function<void(Move)> mRemoteMoveCallback;

	const std::string		  mSecret = "316";

	struct DiscoveredOpponent
	{
		std::string		  name;
		netlink::Endpoint endpoint;
	};

	std::vector<DiscoveredOpponent> mDiscoveredOpponents;
	mutable std::mutex				mMutex;

	bool							mInitialized{false};
};
