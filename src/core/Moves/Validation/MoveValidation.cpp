/*
  ==============================================================================
	Module:         MoveValidation
	Description:    Move legality validation
  ==============================================================================
*/

#include "MoveValidation.h"
#include "BitboardUtils.h"


MoveValidation::MoveValidation(Chessboard &board, MoveGeneration &generation, MoveExecution &execution) : mBoard(board), mGeneration(generation), mExecution(execution) {}


bool MoveValidation::isInCheck() const
{
	return isKingAttacked(mBoard.getCurrentSide());
}


bool MoveValidation::isKingAttacked(const Side side) const
{
	const Square kingPos = getKingSquare(side);

	if (kingPos == Square::None)
		return false;

	const Side attacker = (side == Side::White) ? Side::Black : Side::White;
	return mGeneration.isSquareAttacked(kingPos, attacker);
}


bool MoveValidation::isMoveLegal(const Move move) const
{
	// Make move
	if (!mExecution.makeMove(move))
		return false;

	// Check if the side that just moved left king in check
	// Note: makeMove flips the side, so we use the opposite side
	const Side movedSide   = (mBoard.getCurrentSide() == Side::White) ? Side::Black : Side::White;

	const bool kingInCheck = isKingAttacked(movedSide);

	mExecution.unmakeMove();

	return !kingInCheck;
}


bool MoveValidation::isCheckmate() const
{
	if (!isInCheck())
		return false;

	return countLegalMoves() == 0;
}


bool MoveValidation::isStalemate() const
{
	if (isInCheck())
		return false;

	return countLegalMoves() == 0;
}


bool MoveValidation::isDraw() const
{
	return getDrawReason() != DrawReason::None;
}


DrawReason MoveValidation::getDrawReason() const
{
	// 50-move rule
	if (mBoard.getHalfMoveClock() >= 100)
		return DrawReason::FiftyMoveRule;

	// Insufficient material
	if (hasInsufficientMaterial())
		return DrawReason::InsufficientMaterial;

	// Threefold repetition
	if (mExecution.isRepeatedPosition(3))
		return DrawReason::ThreefoldRepetition;

	return DrawReason::None;
}


void MoveValidation::generateLegalMoves(MoveList &legalMoves) const
{
	MoveList pseudoMoves;
	mGeneration.generateAllMoves(pseudoMoves);

	legalMoves.clear();

	for (const auto pseudoMove : pseudoMoves)
	{
		if (isMoveLegal(pseudoMove))
			legalMoves.push(pseudoMove);
	}
}


size_t MoveValidation::countLegalMoves() const
{
	MoveList pseudoMoves;
	mGeneration.generateAllMoves(pseudoMoves);

	size_t count = 0;

	for (const auto pseudoMove : pseudoMoves)
	{
		if (isMoveLegal(pseudoMove))
		{
			count++;
			// return count;	// TODO: Early exit for optimization
		}
	}

	return count;
}


Square MoveValidation::getKingSquare(const Side side) const
{
	const PieceType kingType = (side == Side::White) ? WKing : BKing;
	const U64		kingBB	 = mBoard.pieces()[kingType];

	if (!kingBB)
		return Square::None;

	return static_cast<Square>(BitUtils::lsb(kingBB));
}


bool MoveValidation::hasInsufficientMaterial() const
{
	const auto &pieces = mBoard.pieces();

	// Any pawns, rooks or queens -> sufficient material
	if (pieces[WPawn] || pieces[BPawn] || pieces[WRook] || pieces[BRook] || pieces[WQueen] || pieces[BQueen])
		return false;

	const int whiteBishops = BitUtils::popCount(pieces[WBishop]);
	const int blackBishops = BitUtils::popCount(pieces[BBishop]);
	const int whiteKnights = BitUtils::popCount(pieces[WKnight]);
	const int blackKnights = BitUtils::popCount(pieces[BKnight]);

	const int whiteMinors  = whiteBishops + whiteKnights;
	const int blackMinors  = blackBishops + blackKnights;

	// K vs K
	if (whiteMinors == 0 && blackMinors == 0)
		return true;

	// K + single minor vs K
	if ((whiteMinors == 1 && blackMinors == 0) || (whiteMinors == 0 && blackMinors == 1))
		return true;

	// K + B vs K + B (treated as insufficient regardless of Bishop Color)
	if (whiteMinors == 1 && blackMinors == 1 && whiteBishops == 1 && blackBishops == 1)
		return true;

	return false;
}
