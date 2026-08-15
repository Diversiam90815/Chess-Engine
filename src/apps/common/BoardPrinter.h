/*
  ==============================================================================
	Module:         BoardPrinter
	Description:    Printing utilities for board and bitboard visualization
  ==============================================================================
*/

#pragma once

#include <vector>

#include "BitboardTypes.h"
#include "ChessBoard.h"

namespace BoardPrinter
{

/**
 * @brief	How to render the board. The defaults reproduce the plain diagnostic
 *			dump used by the perft app.
 */
struct Options
{
	/// Draw from Black's point of view (h1 top-left).
	bool				flip = false;

	/// Use figurine characters instead of ASCII letters. Needs a UTF-8 console.
	bool				unicode = false;

	/// Squares to mark as move targets.
	std::vector<Square> highlight;

	/// Print the side to move, en passant square and castling rights footer.
	bool				showStatus = true;
};


void printBoard(const Chessboard &board);
void printBoard(const Chessboard &board, const Options &options);
void printBitboard(U64 bitboard);

} // namespace BoardPrinter
