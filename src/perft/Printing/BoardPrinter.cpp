/*
  ==============================================================================
	Module:         BoardPrinter
	Description:    Printing utilities for board and bitboard visualization
  ==============================================================================
*/

#include "BoardPrinter.h"
#include "BitboardUtils.h"
#include <cstdio>


namespace BoardPrinter
{

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
	printf("\n");
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			if (!file)
				printf(" %d ", 8 - rank);

			int piece = -1;
			for (int bbPiece = 0; bbPiece < 12; ++bbPiece)
			{
				if (BitUtils::getBit(board.pieces()[bbPiece], square))
					piece = bbPiece;
			}

#ifdef WIN32
			printf(" %c", (piece == -1) ? '.' : asciiPieces[piece]);
#else
			printf(" %s", (piece == -1) ? "." : unicodePieces[piece]);
#endif
		}
		printf("\n");
	}

	printf("\n    a b c d e f g h\n\n");
	printf("   Side:      %s\n", board.getCurrentSide() == Side::White ? "White" : board.getCurrentSide() == Side::Black ? "Black" : "--");
	printf("   Enpassant: %s\n", board.getCurrentEnPassantSqaure() != Square::None ? square_to_coordinates[to_index(board.getCurrentEnPassantSqaure())] : "no");
	printf("   Castling:  %c%c%c%c\n\n", static_cast<int>(board.getCurrentCastlingRights() & Castling::WK) ? 'K' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::WQ) ? 'Q' : '-', static_cast<int>(board.getCurrentCastlingRights() & Castling::BK) ? 'k' : '-',
		   static_cast<int>(board.getCurrentCastlingRights() & Castling::BQ) ? 'q' : '-');
}

} // namespace BoardPrinter
