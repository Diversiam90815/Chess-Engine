/*
  ==============================================================================
	Module:			CPUPlayer
	Description:    Managing the CPU opponent player
  ==============================================================================
*/

#include "CPUPlayer.h"
#include "Evaluation.h"
#include "EngineLog.h"


constexpr int INF				   = std::numeric_limits<int>::max();
constexpr int NEG_INF			   = std::numeric_limits<int>::min() + 1;
constexpr int MAX_QUIESCENCE_DEPTH = 8;


CPUPlayer::CPUPlayer(GameEngine &engine) : mEngine(engine), mRandomGenerator(mRandomDevice()) {}


CPUPlayer::~CPUPlayer()
{
	cancelCalculation();
}


void CPUPlayer::configure(const CPUConfiguration &config)
{
	mConfig = config;
	LOG_INFO("CPU player configured:");
	LOG_INFO("\tDifficulty:\t{}", static_cast<int>(config.difficulty));
	LOG_INFO("\tPlayer:\t{}", sideToString(config.cpuColor));
	LOG_INFO("\tEnabled:\t{}", config.enabled ? "true" : "false");

	clearTranspositionTable();
}


void CPUPlayer::calculateMoveAsync(const std::function<void(Move)> &callback)
{
	cancelCalculation();

	mIsCalculating.store(true);

	mSearchThread = std::jthread(
		[this, callback](const std::stop_token &stopToken)
		{
			const Move bestMove = computeBestMove(stopToken);
			mIsCalculating.store(false);

			if (!stopToken.stop_requested() && callback)
				callback(bestMove);
		});
}


Move CPUPlayer::calculateMove()
{
	const std::stop_source stopSource;
	return computeBestMove(stopSource.get_token());
}


void CPUPlayer::cancelCalculation()
{
	if (mSearchThread.joinable())
	{
		mSearchThread.request_stop();
		mSearchThread.join();
	}
	mIsCalculating.store(false);
}


Move CPUPlayer::computeBestMove(const std::stop_token &stopToken)
{
	// Snapshot the board so the search operates on an independent copy
	mSearchEngine.snapshotFrom(mEngine);

	MoveList legalMoves;
	mSearchEngine.generateLegalMoves(legalMoves);

	if (legalMoves.empty())
	{
		LOG_WARNING("CPU has no legal moves");
		return {};
	}

	if (legalMoves.size() == 1)
	{
		LOG_INFO("CPU has only one legal move.");
		return legalMoves[0];
	}

	mNodesSearched	   = 0;
	mTranspositionHits = 0;
	mMoveEvaluation.clearSearchState();

	const Move bestMove = searchAlphaBeta(legalMoves, getSearchDepth(), stopToken);

	LOG_INFO("CPU searched {} nodes, {} TranspositionHits", mNodesSearched, mTranspositionHits);

	return bestMove;
}


Move CPUPlayer::searchAlphaBeta(const MoveList &moves, const int depth, const std::stop_token &stopToken)
{
	std::vector<ScoredMove> scoredMoves;
	scoredMoves.reserve(moves.size());

	// Check TT for a best-move hint for root ordering
	const uint64_t hash = mSearchEngine.getHash();
	int			   ttScoreUnused{0};
	Move		   ttMove{};
	lookupTransposition(hash, 0, NEG_INF, INF, ttScoreUnused, ttMove);

	// Copy and order moves
	MoveList orderedMoves = moves;

	mMoveEvaluation.orderMoves(orderedMoves, mSearchEngine.getBoard(), ttMove, 0);

	int			  alpha = NEG_INF;
	constexpr int beta	= INF;

	for (const auto move : orderedMoves)
	{
		if (isCancelled(stopToken))
			break;

		if (!mSearchEngine.makeMoveUnchecked(move))
			continue;

		const int score = -alphaBeta(depth - 1, -beta, -alpha, 1, stopToken);
		mSearchEngine.undoMoveUnchecked();

		scoredMoves.push_back({.move = move, .score = score});

		if (score > alpha)
			alpha = score;
	}

	if (scoredMoves.empty())
		return {};

	if (mConfig.enableRandomization)
		return selectWithRandomization(scoredMoves);

	return selectBestMove(scoredMoves);
}


int CPUPlayer::alphaBeta(const int depth, int alpha, const int beta, const int ply, const std::stop_token &stopToken)
{
	if (isCancelled(stopToken))
		return 0;

	++mNodesSearched;

	// Check transposition table
	const uint64_t hash = mSearchEngine.getHash();
	int			   ttScore{0};
	Move		   ttMove{};

	if (lookupTransposition(hash, depth, alpha, beta, ttScore, ttMove))
	{
		++mTranspositionHits;
		return ttScore;
	}

	MoveList moves;
	mSearchEngine.generateLegalMoves(moves);

	// Checkmate/Stalemate
	if (moves.empty())
	{
		if (mSearchEngine.isInCheck())
			return NEG_INF + ply; // prefer faster checkmate

		return 0;				  // stalemate
	}

	if (depth <= 0)
		return quiescence(alpha, beta, stopToken, 0);

	// order moves
	mMoveEvaluation.orderMoves(moves, mSearchEngine.getBoard(), ttMove, ply);

	Move bestMove{};
	auto nodeType = TranspositionEntry::NodeType::UpperBound;

	for (const auto move : moves)
	{
		if (isCancelled(stopToken))
			break;

		if (!mSearchEngine.makeMoveUnchecked(move))
			continue;

		const int score = -alphaBeta(depth - 1, -beta, -alpha, ply + 1, stopToken);
		mSearchEngine.undoMoveUnchecked();

		if (score >= beta)
		{
			// notify MoveEvaluation of the cutoff for killer/history updates
			mMoveEvaluation.updateHistory(move, depth);
			mMoveEvaluation.updateKillerMove(move, depth);

			storeTransposition(hash, depth, beta, TranspositionEntry::NodeType::LowerBound, move);
			return beta; // beta cutoff
		}
		if (score > alpha)
		{
			alpha	 = score;
			bestMove = move;
			nodeType = TranspositionEntry::NodeType::Exact;
		}
	}

	storeTransposition(hash, depth, alpha, nodeType, bestMove);

	return alpha;
}


int CPUPlayer::quiescence(int alpha, const int beta, const std::stop_token &stopToken, const int qDepth)
{
	if (isCancelled(stopToken))
		return 0;

	++mNodesSearched;

	const int standPat = Evaluation::evaluate(mSearchEngine.getBoard());

	if (standPat >= beta)
		return beta;

	if (standPat > alpha)
		alpha = standPat;

	// depth limit prevents quiescence explosion
	if (qDepth >= MAX_QUIESCENCE_DEPTH)
		return alpha;

	// generate only capture moves
	MoveList moves{};
	mSearchEngine.generateLegalMoves(moves);

	MoveList captures{};
	for (auto move : moves)
	{
		if (move.isCapture())
			captures.push(move);
	}

	// Order captures by MVV-LVA
	MoveEvaluation::orderCaptures(captures, mSearchEngine.getBoard());

	for (const auto &move : captures)
	{
		if (!mSearchEngine.makeMoveUnchecked(move))
			continue;

		const int score = -quiescence(-beta, -alpha, stopToken, qDepth + 1);
		mSearchEngine.undoMoveUnchecked();

		if (score >= beta)
			return beta;

		if (score > alpha)
			alpha = score;
	}

	return alpha;
}


Move CPUPlayer::selectBestMove(std::vector<ScoredMove> &scoredMoves)
{
	if (scoredMoves.empty())
		return {};

	const auto best = std::ranges::max_element(scoredMoves, {}, &ScoredMove::score);

	return best->move;
}


Move CPUPlayer::selectWithRandomization(std::vector<ScoredMove> &scoredMoves)
{
	if (scoredMoves.empty())
		return {};

	// sort descending by score
	std::ranges::sort(scoredMoves, std::greater{}, &ScoredMove::score);

	const auto topMoves = filterTopCandidates(scoredMoves, 5);

	if (topMoves.empty())
		return scoredMoves[0].move;

	std::uniform_int_distribution<size_t> dist(0, topMoves.size() - 1);

	return topMoves[dist(mRandomGenerator)].move;
}


std::vector<ScoredMove> CPUPlayer::filterTopCandidates(const std::vector<ScoredMove> &moves, const size_t maxCount)
{
	if (moves.empty())
		return {};

	const int				bestScore = moves[0].score;
	constexpr int			threshold = 50;

	std::vector<ScoredMove> filtered;

	for (const auto &sm : moves)
	{
		if (filtered.size() >= maxCount)
			break;

		if (bestScore - sm.score <= threshold)
			filtered.push_back(sm);
	}

	return filtered;
}


void CPUPlayer::storeTransposition(const uint64_t hash, const int depth, const int score, const TranspositionEntry::NodeType type, const Move bestMove)
{
	const auto it = mTranspositionTable.find(hash);

	if (it != mTranspositionTable.end() && it->second.depth > depth)
		return; // keep deeper entry

	if (mTranspositionTable.size() >= MAX_TRANSPOSITION_ENTRIES)
	{
		size_t toRemove = MAX_TRANSPOSITION_ENTRIES / 4;
		auto   eraseIt	= mTranspositionTable.begin();

		while (toRemove > 0 && eraseIt != mTranspositionTable.end())
		{
			eraseIt = mTranspositionTable.erase(eraseIt);
			--toRemove;
		}
	}

	mTranspositionTable[hash] = {.hash = hash, .depth = depth, .score = score, .type = type, .bestMove = bestMove};
}


bool CPUPlayer::lookupTransposition(const uint64_t hash, const int depth, const int alpha, const int beta, int &score, Move &bestMove)
{
	const auto it = mTranspositionTable.find(hash);
	if (it == mTranspositionTable.end())
		return false;

	const auto &entry = it->second;

	// Always extract best move for ordering, even if score isn't usable
	bestMove		  = entry.bestMove;

	if (entry.depth < depth)
		return false;

	bestMove = entry.bestMove;

	switch (entry.type)
	{
	case TranspositionEntry::NodeType::Exact:
	{
		score = entry.score;
		return true;
	}
	case TranspositionEntry::NodeType::LowerBound:
	{
		if (entry.score >= beta)
		{
			score = entry.score;
			return true;
		}

		break;
	}
	case TranspositionEntry::NodeType::UpperBound:
	{
		if (entry.score <= alpha)
		{
			score = entry.score;
			return true;
		}
		break;
	}
	}

	return false;
}


void CPUPlayer::clearTranspositionTable()
{
	mTranspositionTable.clear();
	mNodesSearched	   = 0;
	mTranspositionHits = 0;
}


int CPUPlayer::getSearchDepth() const
{
	switch (mConfig.difficulty)
	{
	case CPUDifficulty::Easy: return 2;
	case CPUDifficulty::Medium: return 4;
	case CPUDifficulty::Hard: return mConfig.maxDepth;
	default: return 4;
	}
}
