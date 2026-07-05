/*
  ==============================================================================
	Module:         EvaluationScenario
	Description:    Visualization of the score of the position + per component breakdown
  ==============================================================================
*/ 

#pragma once

#include "IScenario.h"


class EvaluationScenario : public IScenario
{
public:
	std::string name() const override { return "Board Evaluation"; }
	void		run(GameEngine &engine) override;
};