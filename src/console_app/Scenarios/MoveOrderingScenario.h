/*
  ==============================================================================
	Module:         MoveOrderingScenario
	Description:    Visualization which move ranks highest before search
  ==============================================================================
*/

#pragma once

#include "IScenario.h"


class MoveOrderingScenario : public IScenario
{
public:
	std::string name() const override { return "Move Ordering"; }
	void		run(GameEngine &engine) override;
};
