/*
  ==============================================================================
	Module:			CallbackBridge
	Description:    Drains the engine's EventQueue into the host's C callback
  ==============================================================================
*/

#include "CallbackBridge.h"

#include "TypeConversion.h"


CallbackBridge::~CallbackBridge()
{
	attach(nullptr);
}


void CallbackBridge::attach(EventQueue *queue)
{
	if (mQueue == queue)
		return;

	if (mQueue)
		mQueue->setWakeHook(nullptr);

	mQueue = queue;

	if (mQueue)
		mQueue->setWakeHook([this] { drain(); });
}


void CallbackBridge::setDelegate(PFN_CALLBACK callback)
{
	{
		std::lock_guard<std::mutex> lock(mDelegateMutex);
		mDelegate = callback;
	}

	// A host that registers late still gets whatever piled up before it arrived.
	drain();
}


void CallbackBridge::drain()
{
	if (!mQueue)
		return;

	engine::EngineEvent event;
	while (mQueue->poll(event))
	{
		dispatch(event);
	}
}


void CallbackBridge::dispatch(const engine::EngineEvent &event)
{
	std::visit(
		[this]<typename T0>(const T0 &e)
		{
			using T = std::decay_t<T0>;

			if constexpr (std::is_same_v<T, engine::GameStateChanged>)
			{
				UIGamePhase phase = mapToUIPhase(e.state);
				sendToHost(MessageType::GameStateChanged, &phase);
			}
			else if constexpr (std::is_same_v<T, engine::PlayerChanged>)
			{
				int currentPlayer = static_cast<int>(e.side);
				sendToHost(MessageType::PlayerChanged, &currentPlayer);
			}
			else if constexpr (std::is_same_v<T, engine::MoveExecuted>)
			{
				MoveEvent payload{};
				payload.data = e.move.raw();
				CopyStringToBuffer(payload.moveNotation, MAX_STRING_LENGTH, e.notation);
				sendToHost(MessageType::MoveExecuted, &payload);
			}
			else if constexpr (std::is_same_v<T, engine::MoveUndone>)
			{
				sendToHost(MessageType::MoveUndone, nullptr);
			}
			else if constexpr (std::is_same_v<T, engine::BoardStateChanged>)
			{
				sendToHost(MessageType::BoardStateChanged, nullptr);
			}
			else if constexpr (std::is_same_v<T, engine::LegalMovesAvailable>)
			{
				sendToHost(MessageType::LegalMovesCalculated, nullptr);
			}
			else if constexpr (std::is_same_v<T, engine::PromotionRequired>)
			{
				sendToHost(MessageType::PawnPromotion, nullptr);
			}
			else if constexpr (std::is_same_v<T, engine::GameEnded>)
			{
				EndgameStateEvent payload{};
				payload.state  = e.state;
				payload.winner = static_cast<int>(e.winner);
				sendToHost(MessageType::EndGameState, &payload);
			}
			else if constexpr (std::is_same_v<T, engine::PieceCaptured>)
			{
				PlayerCapturedPieceEvent payload{};
				payload.playerColor = e.player;
				payload.pieceType	= e.piece;
				payload.captured	= e.captured;
				sendToHost(MessageType::PlayerCapturedPiece, &payload);
			}
		},
		event);
}


bool CallbackBridge::sendToHost(MessageType type, void *message) const
{
	std::lock_guard lock(mDelegateMutex);

	if (!mDelegate)
		return false;

	mDelegate(static_cast<int>(type), message);
	return true;
}


UIGamePhase CallbackBridge::mapToUIPhase(const GameState state)
{
	switch (state)
	{
	case GameState::Init: return UIGamePhase::Initializing;

	case GameState::WaitingForInput:
	case GameState::WaitingForTarget: return UIGamePhase::PlayerTurn;

	case GameState::WaitingForCPUMove: return UIGamePhase::OpponentTurn;

	case GameState::PawnPromotion: return UIGamePhase::PromotionDialog;

	case GameState::GameOver: return UIGamePhase::GameEnded;

	default: return UIGamePhase::Initializing;
	}
}
