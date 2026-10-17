/*
  ==============================================================================
	Module:         PerftScenario
	Description:    Perft (performance test) to validate move generation
					correctness against known node counts.
  ==============================================================================
*/

#include "PerftScenario.h"
#include <chrono>
#include <cstdio>


// Known perft results from the initial position (https://www.chessprogramming.org/Perft_Results)
static constexpr uint64_t kKnownPerft[] = {0, 20, 400, 8902, 197281, 4865609};


void					  PerftScenario::run(GameEngine &engine)
{
	printf("\n=== Perft Test (start position) ===\n");
	printf("%-8s %-14s %-14s %-10s %s\n", "Depth", "Nodes", "Expected", "Match", "Time");
	printf("%s\n", std::string(60, '-').c_str());

	for (int depth = 1; depth <= mMaxDepth; ++depth)
	{
		engine.resetGame(); // always start from initial position

		auto		   start	= std::chrono::steady_clock::now();
		const uint64_t nodes	= perft(engine, depth);
		const auto	   ms		= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

		const uint64_t expected = (depth < static_cast<int>(std::size(kKnownPerft))) ? kKnownPerft[depth] : 0;
		const bool	   match	= (expected == 0 || nodes == expected);

		printf("%-8d %-14llu %-14llu %-10s %lldms\n", depth, nodes, expected, match ? "OK" : "FAIL", ms);
	}
	printf("\n");
}


uint64_t PerftScenario::perft(GameEngine &engine, const int depth)
{
	if (depth == 0)
		return 1;

	MoveList moves;
	engine.generateLegalMoves(moves);

	uint64_t nodes = 0;
	for (const auto move : moves)
	{
		engine.makeMoveUnchecked(move);
		nodes += perft(engine, depth - 1);
		engine.undoMoveUnchecked();
	}
	return nodes;
}
