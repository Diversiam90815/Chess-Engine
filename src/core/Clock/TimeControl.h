/*
  ==============================================================================
	Module:         TimeControl
	Description:    Time assigned to one player
  ==============================================================================
*/

#pragma once

#include <chrono>


struct TimeControl
{
	using Duration = std::chrono::milliseconds;

	Duration initialTime{0};
	Duration increment{0};
	bool	 enabled{false};

	constexpr TimeControl() = default;

	constexpr TimeControl(Duration initial, Duration perMoveIncrement = Duration::zero()) : initialTime(initial), increment(perMoveIncrement), enabled(initial > Duration::zero())
	{
	}

	static constexpr TimeControl disabled() { return {}; }

	static constexpr TimeControl suddenDeath(Duration initial) { return TimeControl(initial, Duration::zero()); }

	static constexpr TimeControl withIncrement(Duration initial, Duration increment) { return TimeControl(initial, increment); }

	constexpr bool				 isEnabled() const noexcept { return enabled && initialTime > Duration::zero(); }
};
