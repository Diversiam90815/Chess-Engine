/*
  ==============================================================================
	Module:         GameView
	Description:    All console rendering for the chess app
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>

#include "EngineEvent.h"
#include "GameManager.h"


class GameView
{
public:
	explicit GameView(GameManager &engine) : mEngine(engine) {}

	//=========================================================================
	// Presentation Options
	//=========================================================================

	void		toggleOrientation() { mFlip = !mFlip; }
	void		setUnicode(bool enabled) { mUnicode = enabled; }

	//=========================================================================
	// Rendering
	//=========================================================================

	void		printBanner() const;
	void		printBoard() const;
	void		printStatus() const;
	void		printLegalMoves(Square from) const;
	void		printAllLegalMoves() const;
	void		printHelp() const;

	// @param sanLog Notation collected from MoveExecuted events, oldest first.
	void		printHistory(const std::vector<std::string> &sanLog) const;

	//=========================================================================
	// Event Reporting
	//=========================================================================

	void		printMovePlayed(const engine::MoveExecuted &event, Side mover) const;
	void		printCapture(const engine::PieceCaptured &event) const;
	void		printGameEnded(const engine::GameEnded &event) const;

	void		printMessage(const std::string &text) const;
	void		printError(const std::string &text) const;

	//=========================================================================
	// Helpers
	//=========================================================================

	static std::string sideName(Side side);
	static std::string pieceName(PieceType piece);


private:
	GameManager &mEngine;

	bool		 mFlip	  = false;
	bool		 mUnicode = false;
};
