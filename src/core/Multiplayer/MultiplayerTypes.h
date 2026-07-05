/*
  ==============================================================================
	Module:         MultiplayerTypes
	Description:    Types for chess multiplayer game setup and messaging
  ==============================================================================
*/

#pragma once

#include <string>
#include <cstdint>


enum class MultiplayerState
{
	None				= 0,
	Searching			= 1,
	OpponentFound		= 2,
	ConnectionRequested = 3,
	Connected			= 4,
	PlayerSetup			= 5,
	ReadyToStart		= 6,
	InGame				= 7,
	Disconnected		= 8,
	Error				= 9
};


struct MultiplayerEvent
{
	MultiplayerState state{MultiplayerState::None};
	std::string		 remoteName{};
	std::string		 errorMessage{};
};


enum class ChessMessageType : uint32_t
{
	Move		 = 1,
	PlayerChosen = 2,
	PlayerReady	 = 3,
	Chat		 = 4
};
