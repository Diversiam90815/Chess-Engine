/*
  ==============================================================================
	Module:         MovePrinter
	Description:    Printing utilities for moves and attack maps
  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>

#include "BitboardTypes.h"
#include "Moves/Generation/MoveGeneration.h"
#include "Moves/Move.h"

namespace MovePrinter
{

void printAttackedSquares(MoveGeneration &gen, Side side);
void printMoveList(const MoveList &moves);

/**
 * @brief	Lay out pre-formatted move labels (SAN, UCI, whatever the caller wants)
 *			in aligned wrapped columns.
 */
void printMoveTable(const std::vector<std::string> &labels, int columns = 6);

} // namespace MovePrinter
