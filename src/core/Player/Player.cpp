/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#include "Player.h"
#include "EngineLog.h"


Player::Player(const Side color) : mSide(color) {}


Side Player::getPlayerColor() const
{
	return mSide;
}


void Player::setPlayerColor(const Side value)
{
	if (mSide != value)
	{
		mSide = value;
	}
}


void Player::addCapturedPiece(const PieceType piece)
{
	mCapturedPieces.push_back(piece);

	if (mEvents)
		mEvents->push(engine::PieceCaptured{.player = getPlayerColor(), .piece = piece, .captured = true});
}


void Player::removeLastCapturedPiece()
{
	if (mCapturedPieces.empty())
	{
		// Handle the case where there are no captured pieces to remove
		LOG_WARNING("No captured pieces to remove.");
		return;
	}

	// Retrieve and remove the last captured piece
	const PieceType lastCapture = mCapturedPieces.back();
	mCapturedPieces.pop_back();

	if (mEvents)
		mEvents->push(engine::PieceCaptured{.player = getPlayerColor(), .piece = lastCapture, .captured = false});
}


void Player::reset()
{
	mCapturedPieces.clear();
}
