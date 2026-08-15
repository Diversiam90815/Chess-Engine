/*
  ==============================================================================
	Module:         EvaluationScenario
	Description:    Visualization of the score of the position + per component breakdown
  ==============================================================================
*/

#include "EvaluationScenario.h"
#include "BoardPrinter.h"
#include "Evaluation.h"
#include "Moves/Notation/MoveNotation.h"
#include <cstdio>


void EvaluationScenario::run(GameEngine &engine)
{
	BoardPrinter::printBoard(engine.getBoard());

	const Chessboard &board = engine.getBoard();
	int				  score = Evaluation::evaluate(board);

	const char		 *side	= board.getCurrentSide() == Side::White ? "White" : "Black";
	printf("   Evaluation (relative to side to move - %s): %+d cp\n\n", side, score);

	// Play a few moves and show how the score evolves
	printf("   %-6s  %-8s  %s\n", "Move#", "Move", "Score (relative)");
	printf("   %s\n", std::string(35, '-').c_str());

	MoveList moves;
	engine.generateLegalMoves(moves);

	// Show evaluation after each of the first N moves
	constexpr int MAX_PREVIEW = 5;
	int			  shown		  = 0;

	for (size_t i = 0; i < moves.size() && shown < MAX_PREVIEW; ++i)
	{
		if (engine.makeMoveUnchecked(moves[i]))
		{
			int	 s	 = Evaluation::evaluate(engine.getBoard());
			auto uci = MoveNotation::toUCI(moves[i]);
			printf("   %-6zu  %-8s  %+d cp\n", i + 1, uci.c_str(), s);
			engine.undoMoveUnchecked();
			++shown;
		}
	}
	printf("\n");
}
