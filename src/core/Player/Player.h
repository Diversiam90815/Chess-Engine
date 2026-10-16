/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#pragma once

#include <vector>

#include "EventQueue.h"
#include "Logging.h"
#include "Parameters.h"


/**
 * @brief	Lightweight aggregation of per-player state (material score, captured pieces,
 *			color assignment).
 */
class Player
{
public:
	Player() = default;
	Player(Side color);
	~Player() = default;

	Side getPlayerColor() const;
	void setPlayerColor(Side value);

	/**
	 * @brief	Destination for PieceCaptured notifications. May be null.
	 */
	void setEventQueue(EventQueue *events) { mEvents = events; }

	/**
	 * @brief	Record a newly captured opponent piece.
	 */
	void addCapturedPiece(const PieceType piece);

	/**
	 * @brief	Remove the most recently captured piece (undo support).
	 */
	void removeLastCapturedPiece();

	/**
	 * @brief	Reset player state to initial (fresh game).
	 */
	void reset();

	const std::vector<PieceType> &getCapturedPieces() const { return mCapturedPieces; }

private:
	Side				   mSide = Side::None;

	std::vector<PieceType> mCapturedPieces;

	bool				   mIsCurrentTurn = false;

	EventQueue			  *mEvents{nullptr};
};
