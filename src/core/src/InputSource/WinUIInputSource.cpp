/*
  ==============================================================================
	Module:			WinUIInputSource
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#include "WinUIInputSource.h"


void WinUIInputSource::setDelegate(PFN_CALLBACK pDelegate)
{
	std::lock_guard<std::mutex> lock(mDelegateMutex);
	mDelegate = pDelegate;
}


void WinUIInputSource::onAddCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = true;
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void WinUIInputSource::onRemoveLastCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = false;
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void WinUIInputSource::onLegalMovesAvailable(Square from, const MoveList &moves)
{
	sendToUI(MessageType::LegalMovesCalculated, nullptr);
}


void WinUIInputSource::onMoveExecuted(Move move, const std::string &notation)
{
	MoveEvent event{};

	event.data = move.raw();
	HRESULT hr = StringCbCopyA(event.moveNotation, MAX_STRING_LENGTH, notation.c_str());

	sendToUI(MessageType::MoveExecuted, &event);
}


void WinUIInputSource::onMoveUndone()
{
	sendToUI(MessageType::MoveUndone, nullptr);
}


void WinUIInputSource::onPromotionRequired()
{
	sendToUI(MessageType::PawnPromotion, nullptr);
}


void WinUIInputSource::onGameStateChanged(GameState state)
{
	UIGamePhase uiPhase = mapToUIPhase(state);

	sendToUI(MessageType::GameStateChanged, &uiPhase);
}


void WinUIInputSource::onGameEnded(EndGameState state, Side winner)
{
	EndgameStateEvent event;
	event.state	 = state;
	event.winner = static_cast<int>(winner);

	sendToUI(MessageType::EndGameState, &event);
}


void WinUIInputSource::onBoardStateChanged()
{
	sendToUI(MessageType::BoardStateChanged, nullptr);
}


void WinUIInputSource::onPlayerChanged(Side playersTurn)
{
	int currentPlayer = (int)playersTurn;

	sendToUI(MessageType::PlayerChanged, &currentPlayer);
}


void WinUIInputSource::onMultiplayerStateChanged(const MultiplayerEvent &event)
{
	CConnectionEvent cEvent = convertToConnectionEvent(event);

	sendToUI(MessageType::ConnectionStateChanged, &cEvent);
}


void WinUIInputSource::onOpponentFound(const std::string &name)
{
	CConnectionEvent cEvent{};
	cEvent.state = static_cast<int>(MultiplayerState::OpponentFound);
	StringCbCopyA(cEvent.remoteName, MAX_STRING_LENGTH, name.c_str());

	sendToUI(MessageType::OpponentDiscovered, &cEvent);
}


void WinUIInputSource::onRemotePlayerChosen(Side remotePlayer)
{
	sendToUI(MessageType::MultiplayerPlayerChosen, &remotePlayer);
}


bool WinUIInputSource::sendToUI(MessageType type, void *message) const
{
	std::lock_guard lock(mDelegateMutex);

	if (!mDelegate)
		return false;

	mDelegate(static_cast<int>(type), message);
	return true;
}


CConnectionEvent WinUIInputSource::convertToConnectionEvent(const MultiplayerEvent &event)
{
	CConnectionEvent cEvent{};
	cEvent.state = static_cast<int>(event.state);

	if (!event.errorMessage.empty())
	{
		StringCbCopyA(cEvent.errorMessage, MAX_STRING_LENGTH, event.errorMessage.c_str());
	}

	if (!event.remoteName.empty())
	{
		StringCbCopyA(cEvent.remoteName, MAX_STRING_LENGTH, event.remoteName.c_str());
	}

	return cEvent;
}


UIGamePhase WinUIInputSource::mapToUIPhase(GameState state)
{
	switch (state)
	{
	case GameState::Init: return UIGamePhase::Initializing;

	case GameState::WaitingForInput:
	case GameState::WaitingForTarget: return UIGamePhase::PlayerTurn;

	case GameState::WaitingForRemoteMove:
	case GameState::WaitingForCPUMove: return UIGamePhase::OpponentTurn;

	case GameState::PawnPromotion: return UIGamePhase::PromotionDialog;

	case GameState::GameOver: return UIGamePhase::GameEnded;

	default: return UIGamePhase::Initializing;
	}
}
