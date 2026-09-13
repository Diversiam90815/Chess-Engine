/*
  ==============================================================================
	Module:         IGameController
	Description:    Interface for game logic operations
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Move.h"
#include "Parameters.h"
#include "GameConfiguration.h"
#include "Execution/MoveExecution.h"
#include "ClockSnapshot.h"
#include "TimeControl.h"

/**
 * @brief	Interface for game logic operations.
 */
class IGameController
{
public:
	virtual ~IGameController()																						= default;

	//=========================================================================
	// Game Lifecycle
	//=========================================================================

	virtual bool					  initializeGame(GameConfiguration config)										= 0;
	virtual void					  resetGame()																	= 0;

	//=========================================================================
	// Move Operations
	//=========================================================================

	virtual void					  getLegalMovesFromSquare(Square sq, MoveList &moves)							= 0;
	virtual MoveExecutionResult		  executeMove(Move move)														= 0;
	virtual bool					  undoLastMove()																= 0;

	//=========================================================================
	// Queries
	//=========================================================================

	virtual bool					  isPromotionMove(Square from, Square to) const									= 0;
	virtual Move					  findMove(Square from, Square to, PieceType promotion = PieceType::None) const = 0;
	virtual EndGameState			  checkEndGame()																= 0;
	virtual DrawReason				  getDrawReason() const															= 0;

	//=========================================================================
	// Turn Management
	//=========================================================================

	virtual Side					  getCurrentSide() const														= 0;
	virtual bool					  isLocalPlayerTurn() const														= 0;
	virtual void					  switchTurns()																	= 0;

	//=========================================================================
	// CPU
	//=========================================================================

	virtual bool					  isCPUTurn() const																= 0;
	virtual void					  requestCPUMoveAsync()															= 0;

	//=====================================================================
	// Game Clock
	//=====================================================================

	virtual void					  configureClock(TimeControl control)											= 0;
	virtual void					  configureClock(TimeControl white, TimeControl black)							= 0;

	virtual void					  resetClock()																	= 0;
	virtual void					  startClock(Side side)															= 0;
	virtual void					  stopClock()																	= 0;
	virtual void					  pauseClock()																	= 0;
	virtual void					  resumeClock(Side side)														= 0;

	virtual bool					  completeClockMove(Side side)													= 0;

	virtual std::chrono::milliseconds getRemainingTime(Side side) const												= 0;

	virtual bool					  isClockRunning() const														= 0;
	virtual bool					  isClockExpired(Side side) const												= 0;

	virtual ClockSnapshot			  snapshotClock() const															= 0;
	virtual void					  restoreClock(const ClockSnapshot &snapshot)									= 0;

	virtual void					  setTimeoutCallback(std::function<void(Side)> callback)						= 0;
};
