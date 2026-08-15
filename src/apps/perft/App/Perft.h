/*
  ==============================================================================
	Module:         Perft
	Description:    Perft app shell: registers and runs scenarios
  ==============================================================================
*/

#pragma once

#include <memory>
#include <vector>

#include "GameEngine.h"
#include "Scenarios/IScenario.h"


class Perft
{
public:
	Perft();

	void run();

private:
	void									registerScenarios();

	GameEngine								mEngine;
	std::vector<std::unique_ptr<IScenario>> mScenarios;
};