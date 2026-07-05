/*
  ==============================================================================
	Module:         BoardPrinter
	Description:    Printing utilities for board and bitboard visualization
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Chessboard.h"

namespace BoardPrinter
{

void printBoard(const Chessboard &board);
void printBitboard(U64 bitboard);

} // namespace BoardPrinter
