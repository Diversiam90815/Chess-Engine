/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Move evaluation and ordering for alpha-beta search.
					Maintains search-accumulated heuristics (killers, history).
  ==============================================================================
*/

#pragma once

#include "Move.h"
#include "ChessBoard.h"
#include "PieceValues.h"

#include <array>
#include <cstring>


/**
 * @brief	Scores and orders moves for alpha-beta search.
 *
 *			Ordering priority (highest first):
 *			1. TT best-move (from transposition table)
 *			2. Captures scored by MVV-LVA
 *			3. Killer moves (quiet moves that caused beta cutoffs)
 *			4. History heuristic (quiet moves that improved alpha)
 *			5. Remaining quiet moves
 */
class MoveEvaluation
{
public:
	MoveEvaluation()  = default;
	~MoveEvaluation() = default;

	void orderMoves(MoveList &moves, const Chessboard &board, Move ttMove, int ply) const;
	void orderCaptures(MoveList &moves, const Chessboard &board) const;

	void updateKillerMove(Move move, int ply);
	void updateHistory(Move move, int depth);

	void clearSearchState();


private:

	[[nodiscard]] int									   evaluateMove(Move move, const Chessboard &board, Move ttMove, int ply) const;
	[[nodiscard]] static int							   evaluateMVV_LVA(Move move, const Chessboard &board);

	[[nodiscard]] bool									   isKillerMove(Move move, int ply) const;

	[[nodiscard]] int									   getHistoryScore(Move move) const;


	static constexpr int								   SCORE_TT_MOVE   = 10'000'000;
	static constexpr int								   SCORE_CAPTURE   = 5'000'000;
	static constexpr int								   SCORE_KILLER_1  = 4'000'000;
	static constexpr int								   SCORE_KILLER_2  = 3'900'000;
	static constexpr int								   SCORE_PROMOTION = 3'000'000;

	static constexpr int								   MAX_PLY		   = 64;
	static constexpr int								   KILLERS_PER_PLY = 2;

	std::array<std::array<Move, KILLERS_PER_PLY>, MAX_PLY> mKillers{};

	int													   mHistory[64][64]{};
};
