/*
  ==============================================================================
	Module:         MovePrinter
	Description:    Printing utilities for moves and attack maps
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Moves/Generation/MoveGeneration.h"
#include "Moves/Move.h"

namespace MovePrinter
{

void printAttackedSquares(MoveGeneration &gen, Side side);
void printMoveList(const MoveList &moves);

} // namespace MovePrinter
