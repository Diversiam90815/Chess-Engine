/*
  ==============================================================================
	Module:			WinUIInputSource
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#pragma once

#include <combaseapi.h>
#include <strsafe.h>
#include <mutex>

#include "IObserver.h"
#include "PlainCDefines.h"
#include "Player.h"
#include "IInputSource.h"
#include "Notation/MoveNotation.h"


enum class MessageType
{
	EndGameState			= 1,
	PlayerCapturedPiece		= 2,
	PlayerChanged			= 3,
	GameStateChanged		= 4,
	MoveExecuted			= 5,
	MoveUndone				= 6,
	ConnectionStateChanged	= 7,
	MultiplayerPlayerChosen = 8,
	BoardStateChanged		= 9,
	PawnPromotion			= 10,
	LegalMovesCalculated	= 11,
	OpponentDiscovered		= 12,
};


struct PlayerCapturedPieceEvent
{
	Side	  playerColor;
	PieceType pieceType;
	bool	  captured;
};


struct MoveEvent
{
	uint16_t data;
	char	 moveNotation[MAX_STRING_LENGTH];
};


struct EndgameStateEvent
{
	EndGameState state;
	int			 winner = static_cast<int>(Side::None);
};


class WinUIInputSource : public IInputSource, public IPlayerObserver, public IMultiplayerObserver
{
public:
	WinUIInputSource()	= default;
	~WinUIInputSource() = default;

	void setDelegate(PFN_CALLBACK callback);

	//=========================================================================
	// IInputSource Implementation (from StateMachine)
	//=========================================================================

	void onLegalMovesAvailable(Square from, const MoveList &moves) override;
	void onMoveExecuted(Move move, const std::string &notation) override;
	void onMoveUndone() override;
	void onPromotionRequired() override;
	void onGameStateChanged(GameState state) override;
	void onGameEnded(EndGameState state, Side winner) override;
	void onBoardStateChanged() override;
	void onPlayerChanged(Side playersTurn) override;

	//=========================================================================
	// IPlayerObserver (scores, captured pieces)
	//=========================================================================

	void onAddCapturedPiece(Side player, PieceType captured) override;
	void onRemoveLastCapturedPiece(Side player, PieceType captured) override;

	//=========================================================================
	// IMultiplayerObserver
	//=========================================================================

	void onMultiplayerStateChanged(const MultiplayerEvent &event) override;
	void onOpponentFound(const std::string &name) override;
	void onRemotePlayerChosen(Side remotePlayer) override;


private:
	bool			   sendToUI(MessageType type, void *message) const;

	CConnectionEvent   convertToConnectionEvent(const MultiplayerEvent &event);

	UIGamePhase		   mapToUIPhase(GameState state);


	PFN_CALLBACK	   mDelegate = nullptr;
	mutable std::mutex mDelegateMutex;
};
