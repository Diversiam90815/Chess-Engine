/*
  ==============================================================================
	Module:         Engine API
	Description:    Plain C API for the Chess engine
  ==============================================================================
*/

#include "EngineAPI.h"

#include "EngineInstance.h"
#include "EngineLog.h"
#include "TypeConversion.h"


//=============================================
//			Core Engine Lifecycle
//=============================================


Engine_API void Init()
{
	EngineInstance::Startup();
}


Engine_API void Deinit()
{
	EngineInstance::Shutdown();
}


Engine_API void SetDelegate(PFN_CALLBACK pDelegate)
{
	if (auto *bridge = EngineInstance::Bridge())
		bridge->setDelegate(pDelegate);
}


//=============================================
//			Logging
//=============================================

Engine_API void SetLogCallback(PFN_LOG_CALLBACK callback, void *context)
{
	if (!callback)
	{
		engine::setLogSink({});
		return;
	}

	engine::setLogSink([callback, context](const engine::LogRecord &record)
					   { callback(static_cast<int>(record.level), record.file, record.line, record.function, record.message.c_str(), context); });
}


Engine_API void SetLogLevel(int level)
{
	if (level < static_cast<int>(engine::LogLevel::Trace))
		level = static_cast<int>(engine::LogLevel::Trace);

	if (level > static_cast<int>(engine::LogLevel::Critical))
		level = static_cast<int>(engine::LogLevel::Critical);

	engine::setLogLevel(static_cast<engine::LogLevel>(level));
}


//=============================================
//			Game Management
//=============================================

Engine_API void StartGame(CGameConfiguration config)
{
	if (auto *engine = EngineInstance::Engine())
		engine->startGame(ToGameConfiguration(config));
}


Engine_API void ResetGame()
{
	if (auto *engine = EngineInstance::Engine())
		engine->resetGame();
}


Engine_API void UndoMove()
{
	if (auto *engine = EngineInstance::Engine())
		engine->undoMove();
}


Engine_API bool GetBoardState(int *boardState)
{
	if (!boardState)
		return false;

	auto *engine = EngineInstance::Engine();
	if (!engine)
		return false;

	std::array<PieceType, 64> pieces = engine->getBoardPieces();

	for (int sq = 0; sq < 64; ++sq)
	{
		boardState[sq] = static_cast<int>(pieces[sq]);
	}

	return true;
}


Engine_API int GetGameState()
{
	auto *engine = EngineInstance::Engine();
	return engine ? static_cast<int>(engine->getGameState()) : 0;
}


Engine_API int GetCurrentSide()
{
	auto *engine = EngineInstance::Engine();
	return engine ? static_cast<int>(engine->getCurrentSide()) : static_cast<int>(Side::None);
}


Engine_API bool IsInCheck()
{
	auto *engine = EngineInstance::Engine();
	return engine ? engine->isInCheck() : false;
}


//=============================================
//			Move Input & Generation
//=============================================

Engine_API void OnSquareSelected(int square)
{
	if (square < 0 || square >= 64)
		return;

	if (auto *engine = EngineInstance::Engine())
		engine->onSquareSelected(static_cast<Square>(square));
}


Engine_API void OnPawnPromotionChosen(int pieceType)
{
	if (auto *engine = EngineInstance::Engine())
		engine->onPromotionChosen(static_cast<PieceType>(pieceType));
}


Engine_API void SubmitMove(int from, int to, int promotion)
{
	if (from < 0 || from >= 64 || to < 0 || to >= 64)
		return;

	if (auto *engine = EngineInstance::Engine())
		engine->submitMove(static_cast<Square>(from), static_cast<Square>(to), static_cast<PieceType>(promotion));
}


Engine_API int GetNumLegalMoves()
{
	auto *engine = EngineInstance::Engine();
	if (!engine)
		return 0;

	return static_cast<int>(engine->getSelectionMoves().size());
}


Engine_API bool GetLegalMoveAtIndex(int index, MoveInstance *move)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !move)
		return false;

	const MoveList &moves = engine->getSelectionMoves();

	if (index < 0 || index >= static_cast<int>(moves.size()))
		return false;

	move->data = moves[index].raw();
	return true;
}


Engine_API int GetLegalMovesForSquare(int square, MoveInstance *moves, int maxMoves)
{
	auto *engine = EngineInstance::Engine();
	if (!engine || !moves || maxMoves <= 0)
		return -1;

	if (square < 0 || square >= 64)
		return -1;

	MoveList found;
	engine->getLegalMovesFromSquare(static_cast<Square>(square), found);

	int count = static_cast<int>(found.size());
	if (count > maxMoves)
		count = maxMoves;

	for (int i = 0; i < count; ++i)
	{
		moves[i].data = found[i].raw();
	}

	return count;
}
