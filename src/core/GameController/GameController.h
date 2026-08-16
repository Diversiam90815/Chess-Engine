/*
  ==============================================================================
	Module:         GameController
	Description:    IGameController implementation
  ==============================================================================
*/

#include <memory>

#include "IGameController.h"
#include "GameEngine.h"
#include "CPUPlayer.h"
#include "GameConfiguration.h"
#include "Player.h"


class GameController : public IGameController
{
public:
	GameController();
	~GameController() override = default;

	//=========================================================================
	// Game Lifecycle
	//=========================================================================

	bool								 initializeGame(GameConfiguration config) override;
	void								 resetGame() override;

	//=========================================================================
	// Move Operations
	//=========================================================================

	void								 getLegalMovesFromSquare(Square sq, MoveList &moves) override;
	MoveExecutionResult					 executeMove(Move move) override;
	bool								 undoLastMove() override;

	//=========================================================================
	// Queries
	//=========================================================================

	// Converts a PieceType to the promotion offset used by Move::promotionPieceOffset()
	// Returns: 0=Knight, 1=Bishop, 2=Rook, 3=Queen, -1=invalid
	static constexpr int				 promotionPieceToOffset(PieceType pt);

	bool								 isPromotionMove(Square from, Square to) const override;
	Move								 findMove(Square from, Square to, PieceType promotion = PieceType::None) const override;
	EndGameState						 checkEndGame() override;
	DrawReason							 getDrawReason() const override;
	bool								 isInCheck() const;

	//=========================================================================
	// Turn Management
	//=========================================================================

	Side								 getCurrentSide() const override;
	bool								 isLocalPlayerTurn() const override;
	void								 switchTurns() override;
	void								 changeCurrentPlayer(Side player);

	//=========================================================================
	// CPU
	//=========================================================================

	bool								 isCPUTurn() const override;
	void								 requestCPUMoveAsync() override;
	void								 cancelCPUCalculation();

	//=========================================================================
	// Accessors
	//=========================================================================

	const Chessboard					&getBoard() const { return mEngine.getBoard(); }
	const std::vector<MoveHistoryEntry> &getMoveHistory() const { return mEngine.getMoveHistory(); }

	/// Legal moves of the most recently selected square (drives UI highlighting).
	const MoveList						&getSelectionMoves() const { return mSelectionMoves; }

	/// Every legal move in the current position.
	const MoveList						&getAllLegalMoves() const;

	std::string							 getMoveNotation(Move move) const { return mEngine.getMoveNotation(move); }

	Player								&getWhitePlayer() { return mWhitePlayer; }
	Player								&getBlackPlayer() { return mBlackPlayer; }

	void								 setEventQueue(EventQueue *events);
	void								 setCPUMoveCallback(std::function<void(Move)> callback);

private:
	GameEngine				  mEngine;
	CPUPlayer				  mCPUPlayer;

	Side					  mLocalPlayer{Side::White};
	GameConfiguration		  mConfig{};

	// Every legal move in the current position (backs findMove / isPromotionMove).
	mutable MoveList		  mCachedLegalMoves;
	mutable bool			  mCacheValid{false};

	// Legal moves of the last square the user selected. Kept apart from the
	// position cache so a selection query cannot narrow what findMove searches.
	MoveList				  mSelectionMoves;

	// Players
	Player					  mWhitePlayer;
	Player					  mBlackPlayer;
	Side					  mCurrentPlayer = Side::None;

	std::function<void(Move)> mOnCPUMove;

	void					  invalidateCache() { mCacheValid = false; }
	void					  ensureCacheValid() const;
};
