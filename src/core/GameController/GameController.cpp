/*
  ==============================================================================
	Module:         GameController
	Description:    IGameController implementation
  ==============================================================================
*/

#include "GameController.h"
#include "EngineLog.h"


GameController::GameController() : mCPUPlayer(mEngine) {}


bool GameController::initializeGame(const GameConfiguration config)
{
	mConfig = config;

	mEngine.init();
	mEngine.resetGame();

	mWhitePlayer.setPlayerColor(Side::White);
	mBlackPlayer.setPlayerColor(Side::Black);

	switch (config.mode)
	{
	case GameModeSelection::LocalCoop:
	{
		mCurrentPlayer = Side::White;

		LOG_INFO("Game initialized: Local coop mode!");
		break;
	}
	case GameModeSelection::SinglePlayer:
	{
		const auto &[humanPlayerColor, aiDifficulty] = config.getSinglePlayer();
		mCurrentPlayer								 = Side::White; // White always starts
		const Side		 cpuColor					 = (humanPlayerColor == Side::White) ? Side::Black : Side::White;

		CPUConfiguration cpuConfig;
		cpuConfig.difficulty		  = aiDifficulty;
		cpuConfig.enabled			  = true;
		cpuConfig.cpuColor			  = cpuColor;
		cpuConfig.enableRandomization = true;

		mCPUPlayer.configure(cpuConfig);

		LOG_INFO("Game initialized: Single Player mode (Human: {}, CPU: {}, Difficulty: {})", sideToString(humanPlayerColor), sideToString(cpuColor),
				 cpuDifficultyToString(aiDifficulty));

		break;
	}
	case GameModeSelection::None: break;
	}

	invalidateCache();

	LOG_INFO("Game initialized successfully!");
	return true;
}


void GameController::resetGame()
{
	mEngine.resetGame();

	mWhitePlayer.reset();
	mBlackPlayer.reset();
	mCurrentPlayer = Side::None;

	mSelectionMoves.clear();
	invalidateCache();
}


void GameController::getLegalMovesFromSquare(const Square sq, MoveList &moves)
{
	mEngine.getMovesFromSquare(sq, moves);

	if (!moves.empty())
		mSelectionMoves = moves;
}


const MoveList &GameController::getAllLegalMoves() const
{
	ensureCacheValid();
	return mCachedLegalMoves;
}


MoveExecutionResult GameController::executeMove(const Move move)
{
	MoveExecutionResult result = mEngine.makeMove(move);

	if (result)
		invalidateCache();

	if (result.capturedPiece != PieceType::None)
	{
		if (const Side currentSide = getCurrentSide(); currentSide == Side::White)
			mWhitePlayer.addCapturedPiece(result.capturedPiece);
		else if (currentSide == Side::Black)
			mBlackPlayer.addCapturedPiece(result.capturedPiece);
	}

	return result;
}


bool GameController::undoLastMove()
{
	// Check if last move was a capture before undoing
	const auto &history	   = mEngine.getMoveHistory();
	const bool	wasCapture = !history.empty() && history.back().move.isCapture();

	if (mEngine.undoMove())
	{
		invalidateCache();

		if (wasCapture)
		{
			// Last move was made by the opponent of mCurrentPlayer
			Side lastMover = (mCurrentPlayer == Side::White) ? Side::Black : Side::White;

			if (lastMover == Side::White)
				mWhitePlayer.removeLastCapturedPiece();
			else if (lastMover == Side::Black)
				mBlackPlayer.removeLastCapturedPiece();
		}
		return true;
	}

	return false;
}


constexpr int GameController::promotionPieceToOffset(const PieceType pt)
{
	switch (pt)
	{
	case WKnight:
	case BKnight: return 0;
	case WBishop:
	case BBishop: return 1;
	case WRook:
	case BRook: return 2;
	case WQueen:
	case BQueen: return 3;
	default: return -1;
	}
}


bool GameController::isPromotionMove(const Square from, const Square to) const
{
	ensureCacheValid();
	return std::ranges::any_of(mCachedLegalMoves, [&](const Move &m) { return m.from() == from && m.to() == to && m.isPromotion(); });
}


Move GameController::findMove(const Square from, const Square to, const PieceType promotion) const
{
	ensureCacheValid();

	const auto it = std::ranges::find_if(mCachedLegalMoves,
										 [&](const Move &m)
										 {
											 if (m.from() != from || m.to() != to)
												 return false;
											 if (promotion != PieceType::None)
											 {
												 return m.isPromotion() && m.promotionPieceOffset() == promotionPieceToOffset(promotion);
											 }
											 return !m.isPromotion();
										 });

	return (it != mCachedLegalMoves.end()) ? *it : Move{};
}


EndGameState GameController::checkEndGame()
{
	return mEngine.checkForEndGameConditions();
}


DrawReason GameController::getDrawReason() const
{
	return mEngine.getDrawReason();
}


bool GameController::isInCheck() const
{
	return mEngine.isInCheck();
}


Side GameController::getCurrentSide() const
{
	return mCurrentPlayer;
}


void GameController::switchTurns()
{
	if (mCurrentPlayer == Side::None)
	{
		changeCurrentPlayer(Side::White);
		return;
	}

	const Side next = (mCurrentPlayer == Side::White) ? Side::Black : Side::White;

	changeCurrentPlayer(next);

	invalidateCache();
}


void GameController::changeCurrentPlayer(const Side player)
{
	if (mCurrentPlayer == player)
		return;

	mCurrentPlayer = player;
}


bool GameController::isCPUTurn() const
{
	if (mConfig.mode != GameModeSelection::SinglePlayer)
		return false;

	return mCurrentPlayer != mConfig.getSinglePlayer().humanPlayerColor;
}


void GameController::requestCPUMoveAsync()
{
	mCPUPlayer.calculateMoveAsync(
		[this](const Move move)
		{
			if (mOnCPUMove)
				mOnCPUMove(move);
		});
}


void GameController::cancelCPUCalculation()
{
	// TODO
}


void GameController::setEventQueue(EventQueue *events)
{
	mWhitePlayer.setEventQueue(events);
	mBlackPlayer.setEventQueue(events);
}


void GameController::setCPUMoveCallback(std::function<void(Move)> callback)
{
	mOnCPUMove = std::move(callback);
}


void GameController::ensureCacheValid() const
{
	if (!mCacheValid)
	{
		mCachedLegalMoves.clear();
		const_cast<GameEngine &>(mEngine).generateLegalMoves(mCachedLegalMoves);
		mCacheValid = true;
	}
}
