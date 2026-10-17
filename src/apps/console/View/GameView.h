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

	void			   toggleOrientation() { mFlip = !mFlip; }
	void			   setUnicode(const bool enabled) { mUnicode = enabled; }

	//=========================================================================
	// Rendering
	//=========================================================================

	static void		   printBanner();
	void			   printBoard() const;
	void			   printStatus() const;
	void			   printLegalMoves(Square from) const;
	void			   printAllLegalMoves() const;
	static void		   printHelp();

	// @param sanLog Notation collected from MoveExecuted events, oldest first.
	static void		   printHistory(const std::vector<std::string> &sanLog);

	//=========================================================================
	// Event Reporting
	//=========================================================================

	static void		   printMovePlayed(const engine::MoveExecuted &event, Side mover);
	static void		   printCapture(const engine::PieceCaptured &event);
	static void		   printGameEnded(const engine::GameEnded &event);

	static void		   printMessage(const std::string &text);
	static void		   printError(const std::string &text);

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
