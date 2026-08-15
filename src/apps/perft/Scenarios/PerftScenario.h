/*
  ==============================================================================
	Module:         PerftScenario
	Description:    Perft (performance test) to validate move generation
					correctness against known node counts.
  ==============================================================================
*/

#pragma once

#include "IScenario.h"
#include <cstdint>


class PerftScenario : public IScenario
{
public:
	explicit PerftScenario(int maxDepth = 4) : mMaxDepth(maxDepth) {}

	std::string name() const override { return "Perft"; }
	void		run(GameEngine &engine) override;

private:
	uint64_t perft(GameEngine &engine, int depth);

	int		 mMaxDepth;
};
