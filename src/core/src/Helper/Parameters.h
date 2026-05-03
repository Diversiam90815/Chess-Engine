/*
  ==============================================================================
	Module:         Parameters
	Description:    Parameters and constants definitions
  ==============================================================================
*/

#pragma once

#include <array>

#include "BitboardTypes.h"


/// <summary>
/// Represents the possible end states of a chess game.
/// </summary>
enum class EndGameState
{
	OnGoing	  = 1,
	Checkmate = 2,
	StaleMate = 3,
	Draw	  = 4,
	Reset	  = 5
};


/**
 * @brief	Representation of CPU difficulty levels
 */
enum class CPUDifficulty
{
	Easy   = 1,
	Medium = 2,
	Hard   = 3
};


/// <summary>
/// Represents the various states of a game used in the StateMachine.
/// </summary>
enum class GameState
{
	Init				 = 1,
	WaitingForInput		 = 2,
	WaitingForTarget	 = 3,
	PawnPromotion		 = 4,
	WaitingForRemoteMove = 5,
	WaitingForCPUMove	 = 6,
	GameOver			 = 7,
};


//============================================================
//			File Manager
//============================================================

namespace FileName
{
constexpr auto LoggingFolder = "Logs";
constexpr auto LogFile		 = "Chess.log";
} // namespace FileName


//============================================================
//			Multiplayer - Network Communication
//============================================================

namespace RemoteControl
{
constexpr int		  PackageBufferSize			   = 65536;
constexpr const char *RemoteComSecret			   = "316";

constexpr auto		  ConnectionStateKey		   = "ConnectionState";
constexpr auto		  MoveKey					   = "Move";
constexpr auto		  ChatMessageKey			   = "Chat";
constexpr auto		  InvitationMessageKey		   = "Invitation";
constexpr auto		  InvitationResponseMessageKey = "InvResponse";
constexpr auto		  PlayerChosenKey			   = "PlayerChosen";
constexpr auto		  PlayerReadyFlagKey		   = "PlayerReady";
} // namespace RemoteControl


//============================================================
//			JSON Conversion
//============================================================

namespace JSONConversion
{
constexpr auto NetworkAdapterID		 = "ID";
constexpr auto NetworkAdapterDesc	 = "adapterName";
constexpr auto NetworkAdapterIP		 = "IPv4";
constexpr auto NetworkAdapterISubnet = "subnet";

constexpr auto DiscoveryIP			 = "IPAddress";
constexpr auto DiscoveryPort		 = "tcpPort";
constexpr auto DiscoveryName		 = "player";

constexpr auto InvitationPlayerName	 = "PlayerName";
constexpr auto InvitationVersion	 = "Version";
constexpr auto InvitationAccepted	 = "Accepted";
constexpr auto InvitationReason		 = "Reason";

constexpr auto ConnectEventType		 = "Type";
constexpr auto ConnectEventError	 = "Error";
constexpr auto ConnectEventEndpoint	 = "Endpoint";
} // namespace JSONConversion
