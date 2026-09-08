/*
  ==============================================================================
	Module:         ClockSnapshot
	Description:    Contains clock values to be stored with move history values
  ==============================================================================
*/

#pragma once

#include <chrono>


struct ClockSnapshot
{
	using Duration = std::chrono::milliseconds;

	Duration					   whiteRemaining{0};
	Duration					   blackRemaining{0};

	bool						   enabled{false};

	static constexpr ClockSnapshot disabled() { return {}; }
};
