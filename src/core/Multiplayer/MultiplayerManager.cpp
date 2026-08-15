/*
  ==============================================================================
	Module:         MultiplayerManager
	Description:    Chess multiplayer session management using NetLink
  ==============================================================================
*/

#include "MultiplayerManager.h"

#include <cstring>


MultiplayerManager::~MultiplayerManager()
{
	shutdown();
}


void MultiplayerManager::init(const std::string &localPlayerName, int discoveryPort)
{
	if (mInitialized)
		return;

	netlink::NetLinkConfig config;
	config.localDisplayName = localPlayerName;
	config.discoveryPort	= discoveryPort;
	config.secret			= mSecret;

	netlink::NetLinkCallbacks callbacks;
	callbacks.onRemoteDiscovered	  = [this](const netlink::Endpoint &remote) { onRemoteDiscovered(remote); };
	callbacks.onConnectionChanged	  = [this](const netlink::ConnectionEvent event) { onConnectionChanged(event); };
	callbacks.onMessageReceived		  = [this](const netlink::Message &message) { onMessageReceived(message); };
	callbacks.onNetworkAdapterChanged = [this](const netlink::NetworkAdapter &adapter) { onAdapterChanged(adapter); };

	mNetLink.configure(config, callbacks);

	if (!mNetLink.init())
	{
		LOG_ERROR("Failed to initialize NetLink");
		return;
	}

	mInitialized = true;
	LOG_INFO("MultiplayerManager initialized (player: {}, port: {})", config.localDisplayName, config.discoveryPort);
}


void MultiplayerManager::shutdown()
{
	if (!mInitialized)
		return;

	mNetLink.disconnect();
	mNetLink.shutdown();

	{
		std::lock_guard lock(mMutex);
		mDiscoveredOpponents.clear();
	}

	mState		 = MultiplayerState::None;
	mInitialized = false;

	LOG_INFO("MultiplayerManager shut down");
}


void MultiplayerManager::findOpponent()
{
	if (!mInitialized)
	{
		LOG_WARNING("Cannot find opponent: MultiplayerManager not initialized");
		return;
	}

	{
		std::lock_guard lock(mMutex);
		mDiscoveredOpponents.clear();
	}

	if (!mNetLink.startDiscovery())
	{
		LOG_ERROR("Failed to start looking for opponents");
		transitionTo(MultiplayerState::Error, "", "Failed to start looking for opponents");
		return;
	}

	transitionTo(MultiplayerState::Searching);
	LOG_INFO("Looking for opponents...");
}


std::vector<std::string> MultiplayerManager::getDiscoveredOpponents()
{
	std::lock_guard			 lock(mMutex);
	std::vector<std::string> names;
	names.reserve(mDiscoveredOpponents.size());
	for (const auto &opponent : mDiscoveredOpponents)
	{
		names.push_back(opponent.name);
	}
	return names;
}


void MultiplayerManager::connectToOpponent(int index)
{
	std::lock_guard lock(mMutex);

	if (index < 0 || index >= static_cast<int>(mDiscoveredOpponents.size()))
	{
		LOG_WARNING("Invalid opponent index: {}", index);
		return;
	}

	const auto &opponent = mDiscoveredOpponents[index];
	LOG_INFO("Connecting to opponent: {}", opponent.name);

	mNetLink.connectTo(opponent.endpoint);
}


void MultiplayerManager::respondToConnectionRequest(bool accept)
{
	mNetLink.respondToConnection(accept);

	if (!accept)
	{
		LOG_INFO("Connection request declined");
	}
}


void MultiplayerManager::disconnect()
{
	mNetLink.disconnect();
	mNetLink.stopDiscovery();
	mLocalPlayerReady  = false;
	mRemotePlayerReady = false;

	{
		std::lock_guard lock(mMutex);
		mDiscoveredOpponents.clear();
	}

	transitionTo(MultiplayerState::Disconnected);
	LOG_INFO("Disconnected from multiplayer session");
}


void MultiplayerManager::sendPlayerChosen(Side localPlayer)
{
	auto msg = serializePlayerChosen(localPlayer);
	mNetLink.send(msg);
	LOG_DEBUG("Sent player choice: {}", static_cast<int>(localPlayer));
}


void MultiplayerManager::sendPlayerReady(bool ready)
{
	mLocalPlayerReady = ready;
	auto msg		  = serializePlayerReady(ready);
	mNetLink.send(msg);
	LOG_DEBUG("Sent ready state: {}", ready);

	if (mLocalPlayerReady && mRemotePlayerReady)
		transitionTo(MultiplayerState::InGame);
}


void MultiplayerManager::sendMove(Move move)
{
	auto msg = serializeMove(move);
	mNetLink.send(msg);
	LOG_DEBUG("Sent move: {} -> {}", static_cast<int>(move.from()), static_cast<int>(move.to()));
}


std::vector<netlink::NetworkAdapter> MultiplayerManager::getAvailableAdapters()
{
	return mNetLink.getAvailableAdapters();
}


bool MultiplayerManager::setActiveAdapter(int adapterID)
{
	return mNetLink.setActiveAdapter(adapterID);
}


int MultiplayerManager::getActiveAdapterID()
{
	return mNetLink.getActiveAdapterID();
}


void MultiplayerManager::setRemoteMoveCallback(std::function<void(Move)> callback)
{
	mRemoteMoveCallback = std::move(callback);
}


MultiplayerState MultiplayerManager::getState() const
{
	return mState;
}


//=========================================================================
// Event Publishing
//=========================================================================

void MultiplayerManager::publishStateChanged(const MultiplayerEvent &event)
{
	if (mEvents)
		mEvents->push(engine::ConnectionChanged{event.state, event.remoteName, event.errorMessage});
}


void MultiplayerManager::publishOpponentFound(const std::string &name)
{
	if (mEvents)
		mEvents->push(engine::OpponentDiscovered{name});
}


void MultiplayerManager::publishRemotePlayerChosen(Side remotePlayer)
{
	if (mEvents)
		mEvents->push(engine::RemotePlayerChosen{remotePlayer});
}


//=========================================================================
// NetLink Callbacks
//=========================================================================

void MultiplayerManager::onRemoteDiscovered(const netlink::Endpoint &remote)
{
	{
		std::lock_guard lock(mMutex);

		for (const auto &existing : mDiscoveredOpponents)
		{
			if (existing.endpoint == remote)
				return;
		}

		mDiscoveredOpponents.push_back({remote.displayName, remote});
	}

	LOG_INFO("Opponent found: {}", remote.displayName);

	transitionTo(MultiplayerState::OpponentFound, remote.displayName);
	publishOpponentFound(remote.displayName);
}


void MultiplayerManager::onConnectionChanged(const netlink::ConnectionEvent event)
{
	switch (event.state)
	{
	case netlink::ConnectionState::PendingInbound: transitionTo(MultiplayerState::ConnectionRequested, event.remote.displayName); break;

	case netlink::ConnectionState::Connected:
		mNetLink.stopDiscovery();
		transitionTo(MultiplayerState::Connected, event.remote.displayName);
		mLocalPlayerReady  = false;
		mRemotePlayerReady = false;
		transitionTo(MultiplayerState::PlayerSetup, event.remote.displayName);
		break;

	case netlink::ConnectionState::Disconnected: transitionTo(MultiplayerState::Disconnected); break;

	case netlink::ConnectionState::Error: transitionTo(MultiplayerState::Error, "", event.errorMessage); break;

	default: break;
	}
}


void MultiplayerManager::onMessageReceived(const netlink::Message &message)
{
	auto type = static_cast<ChessMessageType>(message.type);

	switch (type)
	{
	case ChessMessageType::Move:
	{
		Move move = deserializeMove(message);
		LOG_DEBUG("Received remote move: {} -> {}", static_cast<int>(move.from()), static_cast<int>(move.to()));

		if (mRemoteMoveCallback)
			mRemoteMoveCallback(move);
		break;
	}

	case ChessMessageType::PlayerChosen:
	{
		Side remoteSide = deserializePlayerChosen(message);
		LOG_DEBUG("Remote player chose side: {}", static_cast<int>(remoteSide));
		publishRemotePlayerChosen(remoteSide);
		break;
	}

	case ChessMessageType::PlayerReady:
	{
		bool ready = deserializePlayerReady(message);
		LOG_DEBUG("Remote player ready: {}", ready);
		mRemotePlayerReady = ready;

		if (mLocalPlayerReady && mRemotePlayerReady)
			transitionTo(MultiplayerState::InGame);
		break;
	}

	default: LOG_WARNING("Unknown chess message type: {}", message.type); break;
	}
}


void MultiplayerManager::onAdapterChanged(const netlink::NetworkAdapter &adapter)
{
	LOG_INFO("Network adapter changed: {}", adapter.adapterName);
}


//=========================================================================
// State Management
//=========================================================================

void MultiplayerManager::transitionTo(MultiplayerState newState, const std::string &remoteName, const std::string &error)
{
	mState = newState;

	MultiplayerEvent event;
	event.state		   = newState;
	event.remoteName   = remoteName;
	event.errorMessage = error;

	publishStateChanged(event);
}


//=========================================================================
// Message Serialization
//=========================================================================

netlink::Message MultiplayerManager::serializeMove(Move move)
{
	netlink::Message msg;
	msg.type	 = static_cast<uint32_t>(ChessMessageType::Move);

	uint16_t raw = move.raw();
	msg.data.resize(sizeof(raw));
	std::memcpy(msg.data.data(), &raw, sizeof(raw));

	return msg;
}


netlink::Message MultiplayerManager::serializePlayerChosen(Side side)
{
	netlink::Message msg;
	msg.type = static_cast<uint32_t>(ChessMessageType::PlayerChosen);

	auto val = static_cast<uint8_t>(side);
	msg.data.push_back(val);

	return msg;
}


netlink::Message MultiplayerManager::serializePlayerReady(bool ready)
{
	netlink::Message msg;
	msg.type = static_cast<uint32_t>(ChessMessageType::PlayerReady);

	msg.data.push_back(ready ? 1 : 0);

	return msg;
}


Move MultiplayerManager::deserializeMove(const netlink::Message &msg)
{
	if (msg.data.size() < sizeof(uint16_t))
		return Move::none();

	uint16_t raw = 0;
	std::memcpy(&raw, msg.data.data(), sizeof(raw));

	return Move(raw);
}


Side MultiplayerManager::deserializePlayerChosen(const netlink::Message &msg)
{
	if (msg.data.empty())
		return Side::None;

	return static_cast<Side>(msg.data[0]);
}


bool MultiplayerManager::deserializePlayerReady(const netlink::Message &msg)
{
	if (msg.data.empty())
		return false;

	return msg.data[0] != 0;
}
