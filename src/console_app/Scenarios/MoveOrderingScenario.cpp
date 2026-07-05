/*
  ==============================================================================
	Module:         MoveOrderingScenario
	Description:    Visualization which move ranks highest before search
  ==============================================================================
*/

#include "MoveOrderingScenario.h"
#include "BoardPrinter.h"
#include "Moves/Evaluation/MoveEvaluation.h"
#include "Moves/Notation/MoveNotation.h"
#include <cstdio>


void MoveOrderingScenario::run(GameEngine &engine)
{
	BoardPrinter::printBoard(engine.getBoard());

	MoveList moves;
	engine.generateLegalMoves(moves);

	if (moves.empty())
	{
		printf("   No legal moves available.\n\n");
		return;
	}

	// Order with a fresh MoveEvaluation (no search state — shows base ordering)
	MoveEvaluation eval;
	eval.orderMoves(moves, engine.getBoard(), Move{}, 0);

	printf("   Move ordering (%zu moves, no TT/killer/history state):\n\n", moves.size());
	printf("   %-5s  %-8s  %s\n", "Rank", "UCI", "Flags");
	printf("   %s\n", std::string(35, '-').c_str());

	for (size_t i = 0; i < moves.size(); ++i)
	{
		const Move &m	 = moves[i];
		auto		uci	 = MoveNotation::toUCI(m);
		const char *cap	 = m.isCapture() ? "[capture]" : "";
		const char *prom = m.isPromotion() ? "[promotion]" : "";
		printf("   %-5zu  %-8s  %s%s\n", i + 1, uci.c_str(), cap, prom);
	}
	printf("\n");
}
