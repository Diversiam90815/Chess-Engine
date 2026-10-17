/*
  ==============================================================================
	Module:         GameEngine
	Description:    Core chess engine orchestrating board state and game flow.
  ==============================================================================
*/

#pragma once

#include "Execution/MoveExecution.h"
#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Parameters.h"


class GameEngine
{
public:
	GameEngine();
	~GameEngine() = default;

	// Lifecycle
	void											   init();
	void											   startGame();
	void											   resetGame();

	/**
	 * @brief	Copy board state from another engine (for search isolation).
	 *			Clears move history so the search starts with a clean slate.
	 */
	void											   snapshotFrom(const GameEngine &other);

	//=========================================================================
	// Move Operations
	//=========================================================================

	MoveExecutionResult								   makeMove(Move move);
	bool											   undoMove();


	//=========================================================================
	// Search-Optimized Operations (no validation, notation, or locking)
	//=========================================================================

	bool											   makeMoveUnchecked(Move move);
	bool											   undoMoveUnchecked();


	//=========================================================================
	// Move Generation & Validation
	//=========================================================================

	void											   generateLegalMoves(MoveList &moves);

	bool											   isMoveLegal(Move move);

	void											   getMovesFromSquare(Square from, MoveList &moves);

	//=========================================================================
	//	Game State Queries
	//=========================================================================

	[[nodiscard]] bool								   isInCheck() const;
	bool											   isCheckmate();
	bool											   isStalemate();
	[[nodiscard]] bool								   isDraw() const;
	[[nodiscard]] DrawReason						   getDrawReason() const;
	EndGameState									   checkForEndGameConditions();

	//=========================================================================
	//	End Game
	//=========================================================================

	[[nodiscard]] Side								   getWinner() const;

	//=========================================================================
	//	CPU Player
	//=========================================================================

	uint64_t										   getHash() const;

	//=========================================================================
	//	Accessors
	//=========================================================================

	[[nodiscard]] const Chessboard					  &getBoard() const { return mChessBoard; }
	Chessboard										  &getBoard() { return mChessBoard; }

	[[nodiscard]] const std::vector<MoveHistoryEntry> &getMoveHistory() const;
	[[nodiscard]] std::string						   getMoveNotation(Move move) const;


private:
	Chessboard	   mChessBoard;
	MoveGeneration mMoveGeneration;
	MoveExecution  mMoveExecution;
	MoveValidation mMoveValidation;

	// Thread safety
	std::mutex	   mMoveMutex;
};
