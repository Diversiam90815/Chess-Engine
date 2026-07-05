/*
  ==============================================================================
	Module:         AttackMapScenario
	Description:    Visualizing attacked squares
  ==============================================================================
*/

#pragma once

#include "IScenario.h"


class AttackMapScenario : public IScenario
{
public:
	std::string name() const override { return "Attack Map"; }
	void		run(GameEngine &engine) override;
};
