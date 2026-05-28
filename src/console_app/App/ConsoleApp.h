/*
  ==============================================================================
	Module:         ConsoleApp
	Description:    Console app shell: registers and runs scenarios
  ==============================================================================
*/

#pragma once

#include "GameEngine.h"
#include "Scenarios/IScenario.h"
#include <memory>
#include <vector>


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