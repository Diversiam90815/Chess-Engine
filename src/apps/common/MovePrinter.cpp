/*
  ==============================================================================
	Module:         MovePrinter
	Description:    Printing utilities for moves and attack maps
  ==============================================================================
*/

#include "MovePrinter.h"
#include "Moves/Notation/MoveNotation.h"

#include <algorithm>
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


void printMoveTable(const std::vector<std::string> &labels, int columns)
{
	if (labels.empty())
	{
		printf("  (none)\n");
		return;
	}

	if (columns < 1)
		columns = 1;

	size_t widest = 0;
	for (const auto &label : labels)
		widest = std::max(widest, label.size());

	const int width = static_cast<int>(widest);

	for (size_t i = 0; i < labels.size(); ++i)
	{
		if (i % columns == 0)
			printf("  ");

		printf("%-*s  ", width, labels[i].c_str());

		if ((i + 1) % columns == 0)
			printf("\n");
	}

	if (labels.size() % columns != 0)
		printf("\n");
}

} // namespace MovePrinter
