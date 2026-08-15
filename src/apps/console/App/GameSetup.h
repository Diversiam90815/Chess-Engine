/*
  ==============================================================================
	Module:         GameSetup
	Description:    Interactive prompts that build a GameConfiguration
  ==============================================================================
*/

#pragma once

#include <optional>

#include "GameConfiguration.h"


namespace GameSetup
{

/**
 * @brief	Walk the player through choosing a game mode.
 * @return	The chosen configuration, or nullopt if input ended (Ctrl+Z / EOF).
 */
std::optional<GameConfiguration> run();

} // namespace GameSetup
