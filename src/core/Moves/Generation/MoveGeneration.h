/*
  ==============================================================================
	Module:         MoveGeneration
	Description:    Manages the generation of moves
  ==============================================================================
*/

#pragma once

#include "ChessBoard.h"
#include "Move.h"
#include "BitboardTypes.h"


#define GENERATION_DEBUG false


class MoveGeneration
{
public:
	explicit MoveGeneration(Chessboard &board);
	~MoveGeneration() = default;

	[[nodiscard]] bool isSquareAttacked(Square square, Side attacker) const;
	void			   generateAllMoves(MoveList &moves) const;

private:
	void		generateCastlingMoves(MoveList &moves, Side side) const;
	void		generatePawnMoves(MoveList &moves, Side side) const;
	void		generateKnightMoves(MoveList &moves, Side side) const;
	void		generateRookMoves(MoveList &moves, Side side) const;
	void		generateBishopMoves(MoveList &moves, Side side) const;
	void		generateQueenMoves(MoveList &moves, Side side) const;
	void		generateKingMoves(MoveList &moves, Side side) const;

	static void addSlidingMoves(MoveList &moves, Square from, U64 attacks, U64 enemyOcc);

	Chessboard &mChessBoard;
};
