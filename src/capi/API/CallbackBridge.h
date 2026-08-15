/*
  ==============================================================================
	Module:			CallbackBridge
	Description:    Drains the engine's EventQueue into the host's C callback
  ==============================================================================
*/

#pragma once

#include <mutex>

#include "EngineEvent.h"
#include "EngineTypes.h"
#include "EventQueue.h"


/**
 * @brief	Message ids handed to PFN_CALLBACK as its first argument.
 * @note	These values are part of the ABI - the managed host switches on them.
 *			Never renumber; only append.
 */
enum class MessageType
{
	EndGameState			= 1,
	PlayerCapturedPiece		= 2,
	PlayerChanged			= 3,
	GameStateChanged		= 4,
	MoveExecuted			= 5,
	MoveUndone				= 6,
	ConnectionStateChanged	= 7,
	MultiplayerPlayerChosen = 8,
	BoardStateChanged		= 9,
	PawnPromotion			= 10,
	LegalMovesCalculated	= 11,
	OpponentDiscovered		= 12,
};


// The payload layouts below are mirrored by the managed host. Field types and
// order are ABI - do not change them without updating every host in lockstep.

struct PlayerCapturedPieceEvent
{
	Side	  playerColor;
	PieceType pieceType;
	bool	  captured;
};


struct MoveEvent
{
	uint16_t data;
	char	 moveNotation[MAX_STRING_LENGTH];
};


struct EndgameStateEvent
{
	EndGameState state	= EndGameState::OnGoing;
	int			 winner = static_cast<int>(Side::None);
};


/**
 * @brief	Bridges the engine's pull-based event queue to the host's push-based
 *			delegate.
 *
 *			Attaching installs a wake hook on the queue, so the delegate keeps
 *			firing on the engine thread exactly as it did before the queue existed.
 */
class CallbackBridge
{
public:
	CallbackBridge()  = default;
	~CallbackBridge();

	CallbackBridge(const CallbackBridge &)			   = delete;
	CallbackBridge	 &operator=(const CallbackBridge &) = delete;

	/// Start draining @p queue. Pass nullptr to stop.
	void			   attach(EventQueue *queue);

	void			   setDelegate(PFN_CALLBACK callback);

	/// Deliver everything currently pending. Safe to call from any thread.
	void			   drain();


private:
	void			   dispatch(const engine::EngineEvent &event);
	bool			   sendToHost(MessageType type, void *message) const;

	static UIGamePhase mapToUIPhase(GameState state);

	EventQueue		  *mQueue = nullptr;

	PFN_CALLBACK	   mDelegate = nullptr;
	mutable std::mutex mDelegateMutex;
};
