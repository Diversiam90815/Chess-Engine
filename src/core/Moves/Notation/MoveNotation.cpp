/*
  ==============================================================================
	Module:         MoveNotation
	Description:    Transforming the Move class into a Standart Algebraic Notation string
  ==============================================================================
*/

#include "MoveNotation.h"


std::string MoveNotation::toSAN(const Move &move, const Chessboard &board, const bool isCheck, const bool isCheckmate)
{
	const MoveFlag flags = move.flags();

	std::string	   notation;

	// handle castling
	if (flags == MoveFlag::KingCastle)
		notation = "O-O";
	else if (flags == MoveFlag::QueenCastle)
		notation = "O-O-O";
	else
	{
		const Square	from   = move.from();
		const Square	to	   = move.to();
		const PieceType piece  = board.pieceAt(from);

		const bool		isPawn = (piece == WPawn || piece == BPawn);

		if (!isPawn)
		{
			const char pieceChar = pieceToSANChar(piece);
			if (pieceChar != '\0')
				notation += pieceChar;

			// TODO: Handle disambiguation
		}

		// capture notation
		if (move.isCapture())
		{
			if (isPawn)
				notation += getFile(from); // include starting file for pawn captures

			notation += 'x';
		}

		// destination square
		notation += squareToString(to);

		// promotion
		if (move.isPromotion())
		{
			notation += '=';
			const int			  promoOffset  = move.promotionPieceOffset(); // 0 = Knight, 1 = Bishop, 2 = Rook, 3 = Queen
			static constexpr char promoChars[] = {'N', 'B', 'R', 'Q'};
			notation += promoChars[promoOffset];
		}
	}

	// check/mate indicators
	if (isCheckmate)
		notation += '#';
	else if (isCheck)
		notation += '+';

	return notation;
}


std::string MoveNotation::toUCI(const Move move)
{
	std::string uci;

	uci += squareToString(move.from());
	uci += squareToString(move.to());

	if (move.isPromotion())
	{
		const int			  promoOffset  = move.promotionPieceOffset();
		static constexpr char promoChars[] = {'n', 'b', 'r', 'q'};
		uci += promoChars[promoOffset];
	}

	return uci;
}