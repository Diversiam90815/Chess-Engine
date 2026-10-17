/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include "pch.h"

#include "EventQueue.h"
#include "GameController.h"
#include "StateMachine.h"


class GameManager
{
public:
	GameManager() = default;
	~GameManager();
	GameManager(const GameManager &)									= delete;
	GameManager							&operator=(const GameManager &) = delete;

	//=========================================================================
	// Lifecycle
	//=========================================================================

	bool								 init();
	void								 shutDown();
	[[nodiscard]] bool					 isInitialized() const { return mInitialized; }

	//=========================================================================
	// Host Notification Channel
	//=========================================================================

	EventQueue							&events() { return mEvents; }
	const EventQueue					&events() const { return mEvents; }

	//=========================================================================
	// Game Control
	//=========================================================================

	void								 startGame(GameConfiguration config) const;
	void								 resetGame() const;
	void								 undoMove() const;

	//=========================================================================
	// Input Events
	//=========================================================================

	/**
	 * @brief	Submit a fully specified move. Preferred by hosts that already know
	 *			both squares (console, tests, engine protocols).
	 *			Leave @p promotion as None to be asked via a PromotionRequired event.
	 */
	void								 submitMove(Square from, Square to, PieceType promotion = PieceType::None) const;

	/// Click-driven path: first call selects, second call targets.
	void								 onSquareSelected(Square sq) const;
	void								 onPromotionChosen(PieceType piece) const;

	//=========================================================================
	// Board & Game State Queries
	//=========================================================================

	[[nodiscard]] GameState				 getGameState() const;
	[[nodiscard]] Side					 getCurrentSide() const;
	[[nodiscard]] bool					 isInCheck() const;

	std::array<PieceType, 64>			 getBoardPieces() const;
	PieceType							 getPieceAt(Square sq) const;
	const Chessboard					&getBoard() const;

	// Every legal move in the current position.
	const MoveList						&getLegalMoves() const;

	// Legal moves leaving a single square. Pure query - does not change engine state.
	void								 getLegalMovesFromSquare(Square from, MoveList &out) const;

	// Legal moves of the most recently selected square (drives UI highlighting).
	const MoveList						&getSelectionMoves() const;

	std::string							 getMoveNotation(Move move) const;
	const std::vector<MoveHistoryEntry> &getMoveHistory() const;

	const std::vector<PieceType>		&getCapturedPieces(Side player) const;



private:
	//=========================================================================
	// Core Components
	//=========================================================================

	EventQueue						mEvents;
	std::unique_ptr<GameController> mGameController;
	std::unique_ptr<StateMachine>	mStateMachine;

	//=========================================================================
	// State
	//=========================================================================

	bool							mInitialized{false};
};
