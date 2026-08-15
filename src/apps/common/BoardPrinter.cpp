/*
  ==============================================================================
	Module:         BoardPrinter
	Description:    Printing utilities for board and bitboard visualization
  ==============================================================================
*/

#include "BoardPrinter.h"
#include "BitboardUtils.h"

#include <algorithm>
#include <cstdio>


namespace BoardPrinter
{

namespace
{

int pieceIndexAt(const Chessboard &board, int square)
{
	for (int bbPiece = 0; bbPiece < 12; ++bbPiece)
	{
		if (BitUtils::getBit(board.pieces()[bbPiece], square))
			return bbPiece;
	}

	return -1;
}


bool isHighlighted(const Options &options, int square)
{
	return std::find(options.highlight.begin(), options.highlight.end(), static_cast<Square>(square)) != options.highlight.end();
}

} // namespace


void printBitboard(U64 bitboard)
{
	printf("\n");
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			if (!file)
				printf("  %d ", 8 - rank);
			printf(" %d", BitUtils::getBit(bitboard, square) ? 1 : 0);
		}
		printf("\n");
	}
	printf("\n     a b c d e f g h\n\n");
	printf("     Bitboard: %llud\n\n", bitboard);
}


void printBoard(const Chessboard &board)
{
	Options options;

	// Preserve the historical per-platform glyph choice for the plain dump: the
	// Windows console cannot be assumed to be UTF-8 unless a host opted in.
#ifdef WIN32
	options.unicode = false;
#else
	options.unicode = true;
#endif

	printBoard(board, options);
}


void printBoard(const Chessboard &board, const Options &options)
{
	printf("\n");

	for (int i = 0; i < 8; ++i)
	{
		const int rank = options.flip ? 7 - i : i;

		for (int j = 0; j < 8; ++j)
		{
			const int file	 = options.flip ? 7 - j : j;
			const int square = rank * 8 + file;

			if (!j)
				printf(" %d ", 8 - rank);

			// The cell is always two columns wide: a marker slot followed by the
			// glyph, so highlighting never shifts the grid.
			const char marker = isHighlighted(options, square) ? '*' : ' ';
			const int  piece  = pieceIndexAt(board, square);

			if (options.unicode)
				printf("%c%s", marker, (piece == -1) ? "." : unicodePieces[piece]);
			else
				printf("%c%c", marker, (piece == -1) ? '.' : asciiPieces[piece]);
		}
		printf("\n");
	}

	if (options.flip)
		printf("\n    h g f e d c b a\n\n");
	else
		printf("\n    a b c d e f g h\n\n");

	if (!options.showStatus)
		return;

	printf("   Side:      %s\n", board.getCurrentSide() == Side::White ? "White" : board.getCurrentSide() == Side::Black ? "Black" : "--");
	printf("   Enpassant: %s\n", board.getCurrentEnPassantSqaure() != Square::None ? square_to_coordinates[to_index(board.getCurrentEnPassantSqaure())] : "no");
	printf("   Castling:  %c%c%c%c\n\n", static_cast<int>(board.getCurrentCastlingRights() & Castling::WK) ? 'K' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::WQ) ? 'Q' : '-', static_cast<int>(board.getCurrentCastlingRights() & Castling::BK) ? 'k' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::BQ) ? 'q' : '-');
}

} // namespace BoardPrinter
