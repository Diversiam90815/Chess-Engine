/*
  ==============================================================================
	Module:			MoveExecution
	Description:    Manages the execution of moves in chess
  ==============================================================================
*/

#include "MoveExecution.h"


MoveExecution::MoveExecution(Chessboard &board) : mChessBoard(board) {}


bool MoveExecution::makeMove(const Move move)
{
	const Square	from  = move.from();
	const Square	to	  = move.to();
	const PieceType piece = mChessBoard.pieceAt(from);

	if (piece == PieceType::None)
	{
		LOG_DEBUG("Could not make move, since there is no piece at {}", square_to_coordinates[to_index(from)]);
		return false;
	}

	// Save state before making move
	BoardState		prevState = mChessBoard.saveState();

	// determine side
	const bool		isWhite	  = (piece < 6);
	const PieceType pawnType  = isWhite ? WPawn : BPawn;
	const PieceType rookType  = isWhite ? WRook : BRook;

	// Captures	(En Passant handled later)
	if (move.isCapture() && !move.isEnPassant())
	{
		if (const PieceType captured = mChessBoard.pieceAt(to); captured != -1)
		{
			prevState.capturedPiece = captured;
			mChessBoard.removePiece(captured, to);
		}
	}

	mChessBoard.movePiece(piece, from, to);

	// Handle special moves
	switch (move.flags())
	{
	case MoveFlag::DoublePawnPush:
	{
		mChessBoard.setEnPassantSquare(static_cast<Square>(to_index(to) + (isWhite ? 8 : -8)));
		break;
	}
	case MoveFlag::EnPassant:
	{
		const auto		capturedSquare = static_cast<Square>(to_index(to) + (isWhite ? 8 : -8));
		const PieceType capturedPawn   = isWhite ? BPawn : WPawn;
		prevState.capturedPiece		   = capturedPawn;
		mChessBoard.removePiece(capturedPawn, capturedSquare);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::KingCastle:
	{
		const Square rookFrom = isWhite ? Square::h1 : Square::h8;
		const Square rookTo	  = isWhite ? Square::f1 : Square::f8;
		mChessBoard.movePiece(rookType, rookFrom, rookTo);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::QueenCastle:
	{
		const Square rookFrom = isWhite ? Square::a1 : Square::a8;
		const Square rookTo	  = isWhite ? Square::d1 : Square::d8;
		mChessBoard.movePiece(rookType, rookFrom, rookTo);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::KnightPromotion:
	case MoveFlag::KnightPromoCapture:
	{
		mChessBoard.removePiece(piece, to);
		mChessBoard.addPiece(isWhite ? WKnight : BKnight, to);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::BishopPromotion:
	case MoveFlag::BishopPromoCapture:
	{
		mChessBoard.removePiece(piece, to);
		mChessBoard.addPiece(isWhite ? WBishop : BBishop, to);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::QueenPromotion:
	case MoveFlag::QueenPromoCapture:
	{
		mChessBoard.removePiece(piece, to);
		mChessBoard.addPiece(isWhite ? WQueen : BQueen, to);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	case MoveFlag::RookPromotion:
	case MoveFlag::RookPromoCapture:
	{
		mChessBoard.removePiece(piece, to);
		mChessBoard.addPiece(isWhite ? WRook : BRook, to);
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	default:
	{
		mChessBoard.setEnPassantSquare(Square::None);
		break;
	}
	}

	// Update castling rights
	auto rights = static_cast<uint8_t>(mChessBoard.getCurrentCastlingRights());
	rights &= sCastlingRightsUpdate[to_index(from)];
	rights &= sCastlingRightsUpdate[to_index(to)];
	mChessBoard.setCastlingRights(static_cast<Castling>(rights));

	// Update half-move clock
	if (piece == pawnType || move.isCapture())
		mChessBoard.setHalfMoveClock(0);
	else
		mChessBoard.setHalfMoveClock(mChessBoard.getHalfMoveClock() + 1);

	// Update move counter (after black's move)
	if (!isWhite)
		mChessBoard.incrementMoveCounter();

	// flip side to move
	mChessBoard.flipSide();

	// Update occupancies
	mChessBoard.updateOccupancies();

	// record move in history
	mHistory.push_back({.move = move, .previousState = prevState});

	return true;
}


bool MoveExecution::unmakeMove()
{
	if (mHistory.empty())
	{
		LOG_DEBUG("No move to unmake!");
		return false;
	}

	const auto &[move, previousState] = mHistory.back();
	const Square from				  = move.from();
	const Square to					  = move.to();

	// Flip side back frist
	mChessBoard.flipSide();

	const bool		isWhite	  = (mChessBoard.getCurrentSide() == Side::White);
	const PieceType pawnType  = isWhite ? WPawn : BPawn;
	const PieceType rookType  = isWhite ? WRook : BRook;

	// get piece at dest.
	const PieceType pieceAtTo = mChessBoard.pieceAt(to);

	// handle promotions
	if (move.isPromotion())
	{
		mChessBoard.removePiece(pieceAtTo, to);
		mChessBoard.addPiece(pawnType, from);
	}
	else
		// move piece back
		mChessBoard.movePiece(pieceAtTo, to, from);

	// restore captured piece
	if (previousState.capturedPiece != PieceType::None)
	{
		if (move.isEnPassant())
		{
			const auto capturedSq = static_cast<Square>(to_index(to) + (isWhite ? 8 : -8));
			mChessBoard.addPiece(previousState.capturedPiece, capturedSq);
		}
		else
			mChessBoard.addPiece(previousState.capturedPiece, to);
	}

	// Undo castling rook movement
	if (move.flags() == MoveFlag::KingCastle)
	{
		const Square rookFrom = isWhite ? Square::f1 : Square::f8;
		const Square rookTo	  = isWhite ? Square::h1 : Square::h8;
		mChessBoard.movePiece(rookType, rookFrom, rookTo);
	}
	else if (move.flags() == MoveFlag::QueenCastle)
	{
		const Square rookFrom = isWhite ? Square::d1 : Square::d8;
		const Square rookTo	  = isWhite ? Square::a1 : Square::a8;
		mChessBoard.movePiece(rookType, rookFrom, rookTo);
	}

	// decrement move counter if black's move was undone
	if (!isWhite)
		mChessBoard.decrementMoveCounter();

	mChessBoard.restoreState(previousState);

	mChessBoard.updateOccupancies();

	mHistory.pop_back();

	return true;
}


const MoveHistoryEntry *MoveExecution::getLastMove() const
{
	if (mHistory.empty())
		return nullptr;

	return &mHistory.back();
}


PieceType MoveExecution::getLastCapturedPiece() const
{
	if (mHistory.empty())
		return PieceType::None;

	if (const auto &[move, previousState] = mHistory.back(); move.isCapture())
		return previousState.capturedPiece;

	return PieceType::None;
}


bool MoveExecution::isRepeatedPosition(int count) const
{
	const uint64_t currentHash = mChessBoard.getHash();
	int			   occurrences = 1;

	for (auto [move, previousState] : mHistory)
	{
		if (previousState.hash == currentHash)
			if (++occurrences >= count)
				return true;

		// Irreversible moves (captures / pawn move/ castling rights loss)
		// mean no earlier position can ever repeat again -> stop early
		if (previousState.capturedPiece != PieceType::None)
			break;
	}

	return false;
}
