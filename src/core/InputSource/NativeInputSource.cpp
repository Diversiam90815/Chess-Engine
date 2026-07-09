/*
  ==============================================================================
	Module:			NativeInputSource
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#include "NativeInputSource.h"

#include <cstring>


namespace
{

void safeStringCopy(char *dest, size_t destSize, const std::string &src)
{
	if (!dest || destSize == 0)
		return;

	std::strncpy(dest, src.c_str(), destSize - 1);
	dest[destSize - 1] = '\0';
}

} // namespace


void NativeInputSource::setDelegate(PFN_CALLBACK pDelegate)
{
	std::lock_guard<std::mutex> lock(mDelegateMutex);
	mDelegate = pDelegate;
}


void NativeInputSource::onAddCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = true;
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void NativeInputSource::onRemoveLastCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = false;
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void NativeInputSource::onLegalMovesAvailable(Square from, const MoveList &moves)
{
	sendToUI(MessageType::LegalMovesCalculated, nullptr);
}


void NativeInputSource::onMoveExecuted(Move move, const std::string &notation)
{
	MoveEvent event{};

	event.data = move.raw();
	safeStringCopy(event.moveNotation, MAX_STRING_LENGTH, notation);

	sendToUI(MessageType::MoveExecuted, &event);
}


void NativeInputSource::onMoveUndone()
{
	sendToUI(MessageType::MoveUndone, nullptr);
}


void NativeInputSource::onPromotionRequired()
{
	sendToUI(MessageType::PawnPromotion, nullptr);
}


void NativeInputSource::onGameStateChanged(GameState state)
{
	UIGamePhase uiPhase = mapToUIPhase(state);

	sendToUI(MessageType::GameStateChanged, &uiPhase);
}


void NativeInputSource::onGameEnded(EndGameState state, Side winner)
{
	EndgameStateEvent event;
	event.state	 = state;
	event.winner = static_cast<int>(winner);

	sendToUI(MessageType::EndGameState, &event);
}


void NativeInputSource::onBoardStateChanged()
{
	sendToUI(MessageType::BoardStateChanged, nullptr);
}


void NativeInputSource::onPlayerChanged(Side playersTurn)
{
	int currentPlayer = (int)playersTurn;

	sendToUI(MessageType::PlayerChanged, &currentPlayer);
}


void NativeInputSource::onMultiplayerStateChanged(const MultiplayerEvent &event)
{
	CConnectionEvent cEvent = convertToConnectionEvent(event);

	sendToUI(MessageType::ConnectionStateChanged, &cEvent);
}


void NativeInputSource::onOpponentFound(const std::string &name)
{
	CConnectionEvent cEvent{};
	cEvent.state = static_cast<int>(MultiplayerState::OpponentFound);
	safeStringCopy(cEvent.remoteName, MAX_STRING_LENGTH, name);

	sendToUI(MessageType::OpponentDiscovered, &cEvent);
}


void NativeInputSource::onRemotePlayerChosen(Side remotePlayer)
{
	sendToUI(MessageType::MultiplayerPlayerChosen, &remotePlayer);
}


bool NativeInputSource::sendToUI(MessageType type, void *message) const
{
	std::lock_guard lock(mDelegateMutex);

	if (!mDelegate)
		return false;

	mDelegate(static_cast<int>(type), message);
	return true;
}


CConnectionEvent NativeInputSource::convertToConnectionEvent(const MultiplayerEvent &event)
{
	CConnectionEvent cEvent{};
	cEvent.state = static_cast<int>(event.state);

	if (!event.errorMessage.empty())
	{
		safeStringCopy(cEvent.errorMessage, MAX_STRING_LENGTH, event.errorMessage);
	}

	if (!event.remoteName.empty())
	{
		safeStringCopy(cEvent.remoteName, MAX_STRING_LENGTH, event.remoteName);
	}

	return cEvent;
}


UIGamePhase NativeInputSource::mapToUIPhase(GameState state)
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
