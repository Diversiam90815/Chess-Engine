/*
  ==============================================================================
	Module:         IScenario
	Description:    Interface for all console app test/visualization scenarios
  ==============================================================================
*/

#pragma once

#include "GameEngine.h"
#include <string>


class IScenario
{
public:
	virtual ~IScenario()						= default;

	virtual std::string name() const			= 0;
	virtual void		run(GameEngine &engine) = 0;
};
