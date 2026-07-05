/*
  ==============================================================================
	Module:         AttackMapScenario
	Description:    Visualizing attacked squares
  ==============================================================================
*/

#include "AttackMapScenario.h"
#include "BoardPrinter.h"
#include "MovePrinter.h"
#include "Moves/Generation/MoveGeneration.h"


void AttackMapScenario::run(GameEngine &engine)
{
	MoveGeneration gen(engine.getBoard());
	MovePrinter::printAttackedSquares(gen, Side::White);
	MovePrinter::printAttackedSquares(gen, Side::Black);
}
