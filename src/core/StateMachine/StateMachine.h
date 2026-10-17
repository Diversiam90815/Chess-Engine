/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#pragma once

#include "pch.h"

#include "Parameters.h"
#include "Move.h"
#include "ThreadBase.h"
#include "IGameController.h"
#include "EventQueue.h"
#include "InputEvent.h"


/**
 * @brief	Pure state machine - only manages transitions.
 *			All game logic delegated to IGameController.
 *			All host notifications published to the EventQueue.
 */
class StateMachine : public ThreadBase
{
public:
	StateMachine() = default;
	~StateMachine() override;

	// Non-copyable
	StateMachine(const StateMachine &)						= delete;
	StateMachine		   &operator=(const StateMachine &) = delete;

	//=========================================================================
	// Dependency Injection
	//=========================================================================

	void					setGameController(IGameController *controller);
	void					setEventQueue(EventQueue *events);

	//=========================================================================
	// Configuration
	//=========================================================================

	void					setVsCPUMode(const bool enabled) { mIsVsCPU.store(enabled); }

	//=========================================================================
	// Event Input (thread-safe, can be called from any thread)
	//=========================================================================

	void					postEvent(const InputEvent &event);

	// Convenience methods
	void					onSquareSelected(Square sq);
	void					onMoveRequested(Square from, Square to, PieceType promotion = PieceType::None);
	void					onPromotionChosen(PieceType piece);
	void					onCPUMoveCalculated(Move move);
	void					onUndoRequested();
	void					onGameStart(GameConfiguration config);
	void					onGameReset();

	//=========================================================================
	// Queries
	//=========================================================================

	[[nodiscard]] GameState getState() const { return mState.load(); }

protected:
	void run() override;

private:
	//=========================================================================
	// Event Processing
	//=========================================================================

	void				   processEvent(const InputEvent &event);

	//=========================================================================
	// State Handlers - return next state
	//=========================================================================

	GameState			   handleInit(const InputEvent &event);
	GameState			   handleWaitingForInput(const InputEvent &event);
	GameState			   handleWaitingForTarget(const InputEvent &event);
	GameState			   handlePawnPromotion(const InputEvent &event);
	GameState			   handleWaitingForCPU(const InputEvent &event);
	GameState			   handleGameOver(const InputEvent &event);

	//=========================================================================
	// Helpers
	//=========================================================================

	void				   transitionTo(GameState newState);
	void				   onStateEnter(GameState enteringState) const;
	[[nodiscard]] GameState			   determineNextTurnState() const;
	bool				   tryExecuteMove(Move move);

	GameState			   resolveAndPlay(Square from, Square to, PieceType promotion);

	void				   publish(engine::EngineEvent event) const;

	//=========================================================================
	// State
	//=========================================================================

	std::atomic<GameState> mState{GameState::Init};
	MoveIntent			   mMoveIntent;
	EndGameState		   mEndgameState{EndGameState::OnGoing};

	//=========================================================================
	// Mode Flags
	//=========================================================================

	std::atomic<bool>	   mIsVsCPU{false};

	//=========================================================================
	// Dependencies (non-owning)
	//=========================================================================

	IGameController		  *mController{nullptr};
	EventQueue			  *mEvents{nullptr};

	//=========================================================================
	// Thread-Safe Event Queue
	//=========================================================================

	std::queue<InputEvent> mEventQueue;
	std::mutex			   mQueueMutex;
};
