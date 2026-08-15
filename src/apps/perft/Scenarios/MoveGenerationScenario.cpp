/*
  ==============================================================================
	Module:         MoveGenerationScenario
	Description:    Generate and visualizing legal moves
  ==============================================================================
*/

#include "MoveGenerationScenario.h"
#include "BoardPrinter.h"
#include "MovePrinter.h"


void MoveGenerationScenario::run(GameEngine &engine)
{
	BoardPrinter::printBoard(engine.getBoard());

	MoveList moves;
	engine.generateLegalMoves(moves);
	MovePrinter::printMoveList(moves);
}
