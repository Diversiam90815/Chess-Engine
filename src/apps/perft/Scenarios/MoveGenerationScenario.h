/*
  ==============================================================================
	Module:         MoveGenerationScenario
	Description:    Generate and visualizing legal moves
  ==============================================================================
*/

#pragma once

#include "IScenario.h"


class MoveGenerationScenario : public IScenario
{
public:
	[[nodiscard]] std::string name() const override { return "Move Generation"; }
	void					  run(GameEngine &engine) override;
};
