/*
  ==============================================================================
	Module:         StateMachine
	Description:    Thread managing the game states
  ==============================================================================
*/

#include "StateMachine.h"


StateMachine::~StateMachine()
{
	stop();
}


void StateMachine::setGameController(IGameController *controller)
{
	mController = controller;
}


void StateMachine::setEventQueue(EventQueue *events)
{
	mEvents = events;
}


void StateMachine::publish(engine::EngineEvent event)
{
	if (mEvents)
		mEvents->push(std::move(event));
}


void StateMachine::postEvent(InputEvent event)
{
	{
		std::lock_guard<std::mutex> lock(mQueueMutex);
		mEventQueue.push(std::move(event));
	}
	triggerEvent();
}


void StateMachine::onSquareSelected(Square sq)
{
	postEvent(InputEvent::SquareSelected(sq));
}


void StateMachine::onMoveRequested(Square from, Square to, PieceType promotion)
{
	postEvent(InputEvent::MoveRequested(from, to, promotion));
}


void StateMachine::onPromotionChosen(PieceType piece)
{
	postEvent(InputEvent::PromotionChosen(piece));
}


void StateMachine::onRemoteMoveReceived(Move move)
{
	postEvent(InputEvent::RemoteMove(move));
}


void StateMachine::onCPUMoveCalculated(Move move)
{
	postEvent(InputEvent::CPUMove(move));
}


void StateMachine::onUndoRequested()
{
	postEvent(InputEvent::Undo());
}


void StateMachine::onGameStart(GameConfiguration config)
{
	postEvent(InputEvent::Start(config));
}


void StateMachine::onGameReset()
{
	postEvent(InputEvent::Reset());
}


void StateMachine::run()
{
	while (isRunning())
	{
		if (!waitForEvent(0))
			break; // Thread was stopped

		if (!isRunning())
			break;

		InputEvent event;
		{
			std::unique_lock lock(mQueueMutex);
			event = std::move(mEventQueue.front());
			mEventQueue.pop();
		}

		processEvent(event);
	}
}


void StateMachine::processEvent(const InputEvent &event)
{
	if (!mController || !mEvents)
	{
		LOG_ERROR("Controller or event queue not set!");
		return;
	}

	GameState currentState = mState.load();
	GameState nextState	   = currentState;

	switch (currentState)
	{
	case GameState::Init: nextState = handleInit(event); break;
	case GameState::WaitingForInput: nextState = handleWaitingForInput(event); break;
	case GameState::WaitingForTarget: nextState = handleWaitingForTarget(event); break;
	case GameState::PawnPromotion: nextState = handlePawnPromotion(event); break;
	case GameState::WaitingForRemoteMove: nextState = handleWaitingForRemote(event); break;
	case GameState::WaitingForCPUMove: nextState = handleWaitingForCPU(event); break;
	case GameState::GameOver: nextState = handleGameOver(event); break;
	default: break;
	}

	if (nextState != currentState)
		transitionTo(nextState);
}


GameState StateMachine::handleInit(const InputEvent &event)
{
	if (event.type == InputEvent::Type::GameStart)
	{
		mIsMultiplayer.store(event.config.mode == GameModeSelection::Multiplayer);
		mIsVsCPU.store(event.config.mode == GameModeSelection::SinglePlayer);

		if (!mController->initializeGame(event.config))
		{
			LOG_ERROR("Failed to initialize game!");
			return GameState::Init;
		}

		mMoveIntent.clear();
		mEndgameState = EndGameState::OnGoing;

		return determineNextTurnState();
	}

	return GameState::Init;
}


GameState StateMachine::handleWaitingForInput(const InputEvent &event)
{
	switch (event.type)
	{
	case InputEvent::Type::SquareSelected:
	{
		MoveList moves;
		mController->getLegalMovesFromSquare(event.square, moves);

		if (moves.size() > 0)
		{
			mMoveIntent.clear();
			mMoveIntent.fromSquare = event.square;
			mMoveIntent.legalMoves = moves;

			publish(engine::LegalMovesAvailable{event.square});
			return GameState::WaitingForTarget;
		}
		break;
	}
	case InputEvent::Type::MoveRequested:
	{
		mMoveIntent.clear();
		mMoveIntent.fromSquare = event.square;

		return resolveAndPlay(event.square, event.toSquare, event.promotion);
	}
	case InputEvent::Type::UndoRequested:
	{
		if (mController->undoLastMove())
		{
			publish(engine::MoveUndone{});
			publish(engine::BoardStateChanged{});

			// The move is off the board, so the turn has to go back with it.
			mController->switchTurns();
			publish(engine::PlayerChanged{mController->getCurrentSide()});
		}
		break;
	}
	case InputEvent::Type::GameReset:
	{
		mController->resetGame();
		mMoveIntent.clear();
		return GameState::Init;
	}
	default: break;
	}

	return GameState::WaitingForInput;
}


GameState StateMachine::handleWaitingForTarget(const InputEvent &event)
{
	if (event.type != InputEvent::Type::SquareSelected)
		return GameState::WaitingForTarget;

	Square sq = event.square;

	// Deselect if clicked on same square
	if (sq == mMoveIntent.fromSquare)
	{
		mMoveIntent.clear();
		publish(engine::BoardStateChanged{}); // Notify host to render a new board
		return GameState::WaitingForInput;
	}

	// Clicked different own piece -> reselect
	MoveList newMoves;
	mController->getLegalMovesFromSquare(sq, newMoves);

	if (newMoves.size() > 0)
	{
		mMoveIntent.clear();
		mMoveIntent.fromSquare = sq;
		mMoveIntent.legalMoves = newMoves;

		publish(engine::LegalMovesAvailable{sq});
		return GameState::WaitingForTarget;
	}

	return resolveAndPlay(mMoveIntent.fromSquare, sq, PieceType::None);
}


GameState StateMachine::resolveAndPlay(Square from, Square to, PieceType promotion)
{
	mMoveIntent.toSquare  = to;
	mMoveIntent.promotion = promotion;

	// Promotion without a piece choice: ask the host and park until it answers.
	if (promotion == PieceType::None && mController->isPromotionMove(from, to))
	{
		publish(engine::PromotionRequired{from, to});
		return GameState::PawnPromotion;
	}

	Move move = mController->findMove(from, to, promotion);

	if (move.isValid() && tryExecuteMove(move))
		return determineNextTurnState();

	// invalid move
	mMoveIntent.clear();
	publish(engine::BoardStateChanged{}); // Notify host to render a new board
	return GameState::WaitingForInput;
}


GameState StateMachine::handlePawnPromotion(const InputEvent &event)
{
	if (event.type == InputEvent::Type::PromotionChosen)
	{
		mMoveIntent.promotion = event.promotion;

		Move move			  = mController->findMove(mMoveIntent.fromSquare, mMoveIntent.toSquare, mMoveIntent.promotion);

		if (move.isValid() && tryExecuteMove(move))
			return determineNextTurnState();

		mMoveIntent.clear();
		publish(engine::BoardStateChanged{});
		return GameState::WaitingForInput;
	}

	return GameState::PawnPromotion;
}


GameState StateMachine::handleWaitingForRemote(const InputEvent &event)
{
	if (event.type == InputEvent::Type::RemoteMove)
	{
		if (tryExecuteMove(event.move))
			return determineNextTurnState();
	}

	return GameState::WaitingForRemoteMove;
}


GameState StateMachine::handleWaitingForCPU(const InputEvent &event)
{
	if (event.type == InputEvent::Type::CPUMove)
	{
		if (tryExecuteMove(event.move))
			return determineNextTurnState();

		LOG_ERROR("CPU returned invalid move");
		return GameState::GameOver;
	}

	return GameState::WaitingForCPUMove;
}


GameState StateMachine::handleGameOver(const InputEvent &event)
{
	if (event.type == InputEvent::Type::GameReset)
	{
		mController->resetGame();
		mMoveIntent.clear();
		mEndgameState = EndGameState::OnGoing;
		return GameState::Init;
	}

	return GameState::GameOver;
}


void StateMachine::transitionTo(GameState newState)
{
	GameState oldState = mState.exchange(newState);

	LOG_INFO("State transition: {} -> {}", Logging::gameStateToString(oldState), Logging::gameStateToString(newState));

	publish(engine::GameStateChanged{newState});

	onStateEnter(newState);
}


void StateMachine::onStateEnter(GameState enteringState)
{
	switch (enteringState)
	{
	case GameState::WaitingForCPUMove:
	{
		mController->requestCPUMoveAsync();
		break;
	}
	default: break;
	}
}


GameState StateMachine::determineNextTurnState()
{
	if (mEndgameState != EndGameState::OnGoing)
	{
		Side	   winner = (mEndgameState == EndGameState::Checkmate) ? (mController->getCurrentSide() == Side::White ? Side::Black : Side::White) : Side::None;
		DrawReason reason = (mEndgameState == EndGameState::Draw) ? mController->getDrawReason() : DrawReason::None;

		publish(engine::GameEnded{mEndgameState, winner, reason});

		return GameState::GameOver;
	}

	if (mIsMultiplayer.load() && !mController->isLocalPlayerTurn())
		return GameState::WaitingForRemoteMove;

	if (mIsVsCPU.load() && mController->isCPUTurn())
		return GameState::WaitingForCPUMove;

	return GameState::WaitingForInput;
}


bool StateMachine::tryExecuteMove(Move move)
{
	auto executionResult = mController->executeMove(move);

	if (!executionResult)
		return false;

	publish(engine::MoveExecuted{move, executionResult.notation});
	publish(engine::BoardStateChanged{});

	mController->switchTurns();
	Side currentSide = mController->getCurrentSide();
	publish(engine::PlayerChanged{currentSide});

	mEndgameState = mController->checkEndGame();

	mMoveIntent.clear();
	return true;
}
