/*
  ==============================================================================
	Module:         GameEngine
	Description:    Core chess engine orchestrating board state and game flow.
  ==============================================================================
*/

#include "GameEngine.h"
#include "EngineLog.h"


GameEngine::GameEngine() : mMoveGeneration(mChessBoard), mMoveExecution(mChessBoard), mMoveValidation(mChessBoard, mMoveGeneration, mMoveExecution) {}


void GameEngine::init()
{
	mChessBoard.init();
}


void GameEngine::startGame()
{
	resetGame();
	LOG_INFO("Game started..");
}


void GameEngine::resetGame()
{
	mChessBoard.init();
	mMoveExecution.clearHistory();
}


void GameEngine::snapshotFrom(const GameEngine &other)
{
	mChessBoard = other.mChessBoard;
	mMoveExecution.clearHistory();
}


MoveExecutionResult GameEngine::makeMove(const Move move)
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	MoveExecutionResult			result;

	result.notation = getMoveNotation(move);

	if (!mMoveValidation.isMoveLegal(move))
	{
		LOG_WARNING("Illegal move attempted: {}", MoveNotation::toUCI(move));
		result.success = false;
		return result;
	}

	if (!mMoveExecution.makeMove(move))
	{
		LOG_ERROR("Move execution failed: {}", MoveNotation::toUCI(move));
		result.success = false;
		return result;
	}

	if (move.isCapture())
		result.capturedPiece = mMoveExecution.getLastCapturedPiece();

	result.success = true;
	return result;
}


bool GameEngine::undoMove()
{
	std::lock_guard<std::mutex> lock(mMoveMutex);

	if (!mMoveExecution.unmakeMove())
	{
		LOG_WARNING("No move to undo!");
		return false;
	}

	return true;
}


bool GameEngine::makeMoveUnchecked(const Move move)
{
	return mMoveExecution.makeMove(move);
}


bool GameEngine::undoMoveUnchecked()
{
	return mMoveExecution.unmakeMove();
}


void GameEngine::generateLegalMoves(MoveList &moves)const
{
	mMoveValidation.generateLegalMoves(moves);
}


bool GameEngine::isMoveLegal(const Move move)const
{
	return mMoveValidation.isMoveLegal(move);
}


void GameEngine::getMovesFromSquare(const Square from, MoveList &moves) const
{
	MoveList allMoves;
	generateLegalMoves(allMoves);

	moves.clear();
	for (auto allMove : allMoves)
	{
		const Square fromVariable = allMove.from();
		LOG_DEBUG("FromVariable: {}", to_index(fromVariable));
		LOG_DEBUG("From: {}", to_index(from));

		if (allMove.from() == from)
			moves.push(allMove);
	}
}


bool GameEngine::isInCheck() const
{
	return mMoveValidation.isInCheck();
}


bool GameEngine::isCheckmate()const
{
	return mMoveValidation.isCheckmate();
}


bool GameEngine::isStalemate()const
{
	return mMoveValidation.isStalemate();
}


bool GameEngine::isDraw() const
{
	return mMoveValidation.isDraw();
}


DrawReason GameEngine::getDrawReason() const
{
	return mMoveValidation.getDrawReason();
}


EndGameState GameEngine::checkForEndGameConditions()
{
	if (mMoveValidation.isCheckmate())
	{
		LOG_INFO("Checkmate!");
		return EndGameState::Checkmate;
	}

	if (mMoveValidation.isStalemate())
	{
		LOG_INFO("Stalemate!");
		return EndGameState::StaleMate;
	}

	if (const DrawReason reason = mMoveValidation.getDrawReason(); reason != DrawReason::None)
	{
		LOG_INFO("Draw! Reason: {}", drawReasonToString(reason));
		return EndGameState::Draw;
	}

	return EndGameState::OnGoing;
}


Side GameEngine::getWinner() const
{
	// Winner is the side that just moved -> opponent got mated
	return (mChessBoard.getCurrentSide() == Side::White) ? Side::Black : Side::White;
}


uint64_t GameEngine::getHash() const
{
	return mChessBoard.getHash();
}


const std::vector<MoveHistoryEntry> &GameEngine::getMoveHistory() const
{
	return mMoveExecution.getHistory();
}


std::string GameEngine::getMoveNotation(Move move) const
{
	return MoveNotation::toSAN(move, mChessBoard, false, false);
}
