/*
  ==============================================================================
	Module:         MovePrinter
	Description:    Printing utilities for moves and attack maps
  ==============================================================================
*/

#include "MovePrinter.h"
#include "Moves/Notation/MoveNotation.h"
#include <cstdio>


namespace MovePrinter
{

void printAttackedSquares(MoveGeneration &gen, Side side)
{
	printf("\nAttacked squares for %s:\n", side == Side::White ? "White" : "Black");
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			if (!file)
				printf("  %d ", 8 - rank);
			printf(" %d", gen.isSquareAttacked(Square(square), side) ? 1 : 0);
		}
		printf("\n");
	}
	printf("\n     a b c d e f g h\n\n");
}


void printMoveList(const MoveList &moves)
{
	printf("\nMove list (%zu moves):\n", moves.size());
	for (size_t i = 0; i < moves.size(); ++i)
		printf("  %3zu. %s\n", i + 1, MoveNotation::toUCI(moves[i]).c_str());
	printf("\n");
}

} // namespace MovePrinter
