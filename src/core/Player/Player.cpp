/*
  ==============================================================================
	Module:         Player
	Description:    Stats and settings for the player
  ==============================================================================
*/


#include "Player.h"


Player::Player(Side color) : mSide(color) {}


Side Player::getPlayerColor() const
{
	return mSide;
}


void Player::setPlayerColor(Side value)
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
		mEvents->push(engine::PieceCaptured{getPlayerColor(), piece, true});
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
	PieceType lastCapture = mCapturedPieces.back();
	mCapturedPieces.pop_back();

	if (mEvents)
		mEvents->push(engine::PieceCaptured{getPlayerColor(), lastCapture, false});
}


void Player::reset()
{
	mCapturedPieces.clear();
}
