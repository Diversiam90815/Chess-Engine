/*
  ==============================================================================
	Module:         ConsoleApp
	Description:    Console app shell: registers and runs scenarios
  ==============================================================================
*/

#pragma once

#include <memory>
#include <vector>

#include "GameEngine.h"
#include "Scenarios/IScenario.h"


class ConsoleApp
{
public:
	ConsoleApp();

	void run();

private:
	void									registerScenarios();

	GameEngine								mEngine;
	std::vector<std::unique_ptr<IScenario>> mScenarios;
};