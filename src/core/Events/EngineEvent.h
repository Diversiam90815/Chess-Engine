/*
  ==============================================================================
	Module:         EngineEvent
	Description:    Host-agnostic notifications published by the engine
  ==============================================================================
*/

#pragma once

#include <string>
#include <variant>

#include "BitboardTypes.h"
#include "Move.h"
#include "MultiplayerTypes.h"
#include "Parameters.h"


/**
 * @brief	Events the engine publishes towards its host
 */
namespace engine
{

// The state machine entered a new state.
struct GameStateChanged
{
	GameState state{GameState::Init};
};


// It is now this side's turn.
struct PlayerChanged
{
	Side side{Side::None};
};


// A move was applied to the board.
struct MoveExecuted
{
	Move		move;
	std::string notation;
};


// The last move was taken back.
struct MoveUndone
{
};


// The board changed; hosts should re-render.
struct BoardStateChanged
{
};


// A piece was selected and has legal moves. Query them via getLegalMovesFromSquare().
struct LegalMovesAvailable
{
	Square from{Square::None};
};


// A pawn reached the last rank; the host must answer with onPromotionChosen().
struct PromotionRequired
{
	Square from{Square::None};
	Square to{Square::None};
};


// The game reached a terminal state.
struct GameEnded
{
	EndGameState state{EndGameState::OnGoing};
	Side		 winner{Side::None};
	DrawReason	 reason{DrawReason::None};
};


// A piece was captured, or a capture was taken back (captured == false).
struct PieceCaptured
{
	Side	  player{Side::None};
	PieceType piece{PieceType::None};
	bool	  captured{true};
};


// A multiplayer opponent was discovered on the network.
struct OpponentDiscovered
{
	std::string name;
};


// The multiplayer connection changed state.
struct ConnectionChanged
{
	MultiplayerState state{MultiplayerState::None};
	std::string		 remoteName;
	std::string		 error;
};


// The remote peer picked a colour.
struct RemotePlayerChosen
{
	Side side{Side::None};
};


using EngineEvent = std::variant<GameStateChanged,
								 PlayerChanged,
								 MoveExecuted,
								 MoveUndone,
								 BoardStateChanged,
								 LegalMovesAvailable,
								 PromotionRequired,
								 GameEnded,
								 PieceCaptured,
								 OpponentDiscovered,
								 ConnectionChanged,
								 RemotePlayerChosen>;

} // namespace engine
