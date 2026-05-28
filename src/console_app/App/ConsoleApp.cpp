/*
  ==============================================================================
	Module:         ConsoleApp
	Description:    Console app shell: registers and runs scenarios
  ==============================================================================
*/

#include "ConsoleApp.h"
#include "Scenarios/AttackMapScenario.h"
#include "Scenarios/MoveGenerationScenario.h"
#include "Scenarios/PerftScenario.h"
#include <cstdio>


ConsoleApp::ConsoleApp()
{
	mEngine.init();
	registerScenarios();
}


void ConsoleApp::registerScenarios()
{
	mScenarios.push_back(std::make_unique<AttackMapScenario>());
	mScenarios.push_back(std::make_unique<MoveGenerationScenario>());
	mScenarios.push_back(std::make_unique<PerftScenario>(5));
}


void ConsoleApp::run()
{
	printf("Chess Engine - Console App\n");
	printf("==========================\n\n");

	for (auto &scenario : mScenarios)
	{
		printf(">>> Running: %s\n", scenario->name().c_str());
		mEngine.resetGame();
		scenario->run(mEngine);
	}

	printf("All scenarios complete.\n");
}
