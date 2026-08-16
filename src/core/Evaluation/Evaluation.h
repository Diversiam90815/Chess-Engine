/*
  ==============================================================================
	Module:         Evaluation
	Description:    Positional Evaluation of the chess board
  ==============================================================================
*/


#pragma once

#include "ChessBoard.h"
#include "BitboardUtils.h"
#include "PieceValues.h"


class Evaluation
{
public:
	Evaluation()  = delete;
	~Evaluation() = delete;

	/**
	 * @brief	Evaluate the current board position.
	 * @param	board	The board to evaluate.
	 * @return	Score in centipawns, positive favoring side to move.
	 */
	[[nodiscard]] static int evaluate(const Chessboard &board);

private:
	[[nodiscard]] static int		   evaluateMaterial(const Chessboard &board);
	[[nodiscard]] static int		   evaluatePieceSquareTables(const Chessboard &board);
	[[nodiscard]] static int		   evaluatePawnStructure(const Chessboard &board);
	[[nodiscard]] static int		   evaluateKingSafety(const Chessboard &board);
	[[nodiscard]] static int		   evaluateMobility(const Chessboard &board);

	[[nodiscard]] static int		   scorePieceSquare(U64 bitboard, const int table[64], bool isWhite);
	[[nodiscard]] static constexpr int mirrorSquare(int sq) { return sq ^ 56; }

	// clang-format off
	static constexpr int PST_PAWN[64] =
	{
		 0,   0,   0,   0,   0,   0,   0,   0,
		50,  50,  50,  50,  50,  50,  50,  50,
		10,  10,  20,  30,  30,  20,  10,  10,
		 5,   5,  10,  25,  25,  10,   5,   5,
		 0,   0,   0,  20,  20,   0,   0,   0,
		 5,  -5, -10,   0,   0, -10,  -5,   5,
		 5,  10,  10, -20, -20,  10,  10,   5,
		 0,   0,   0,   0,   0,   0,   0,   0,
	};

	static constexpr int PST_KNIGHT[64] =
	{
		-50, -40, -30, -30, -30, -30, -40, -50,
		-40, -20,   0,   0,   0,   0, -20, -40,
		-30,   0,  10,  15,  15,  10,   0, -30,
		-30,   5,  15,  20,  20,  15,   5, -30,
		-30,   0,  15,  20,  20,  15,   0, -30,
		-30,   5,  10,  15,  15,  10,   5, -30,
		-40, -20,   0,   5,   5,   0, -20, -40,
		-50, -40, -30, -30, -30, -30, -40, -50,
	};

	static constexpr int PST_BISHOP[64] =
	{
		-20, -10, -10, -10, -10, -10, -10, -20,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   5,   5,  10,  10,   5,   5, -10,
		-10,   0,   5,  10,  10,   5,   0, -10,
		-10,  10,  10,  10,  10,  10,  10, -10,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-20, -10, -10, -10, -10, -10, -10, -20,
	};

	static constexpr int PST_ROOK[64] =
	{
		 0,   0,   0,   0,   0,   0,   0,   0,
		 5,  10,  10,  10,  10,  10,  10,   5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		 0,   0,   0,   5,   5,   0,   0,   0,
	};

	static constexpr int PST_QUEEN[64] =
	{
		-20, -10, -10,  -5,  -5, -10, -10, -20,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,   5,   5,   5,   5,   0, -10,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		  0,   0,   5,   5,   5,   5,   0,  -5,
		-10,   5,   5,   5,   5,   5,   0, -10,
		-10,   0,   5,   0,   0,   0,   0, -10,
		-20, -10, -10,  -5,  -5, -10, -10, -20,
	};

	static constexpr int PST_KING_MIDDLEGAME[64] =
	{
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		 20,  20,   0,   0,   0,   0,  20,  20,
		 20,  30,  10,   0,   0,  10,  30,  20,
	};
	// clang-format on
};
