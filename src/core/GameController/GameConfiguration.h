/*
  ==============================================================================
	Module:         Game Configuration
	Description:    Unifying multiple configuration specs
  ==============================================================================
*/

#pragma once

#include <cassert>

#include "BitboardTypes.h"
#include "Parameters.h"


// Configuration of local two-player mode.
struct LocalCoopConfig
{
	// might add settings later such as time controls, board orientation,...
};


// Configuration for single-player mode against computer AI.
struct SinglePlayerConfig
{
	Side		  humanPlayerColor;
	CPUDifficulty aiDifficulty;
};


//=============================================================================
// Unified Configuration
//=============================================================================

enum class GameModeSelection
{
	None		 = 0,
	LocalCoop	 = 1,
	SinglePlayer = 2,
};


struct GameConfiguration
{
	GameModeSelection mode = GameModeSelection::None;

	// Only one active member at a time; the factory methods below set it explicitly.
	union
	{
		LocalCoopConfig	   localCoop;
		// cppcheck-suppress uninitMemberVarNoCtor
		SinglePlayerConfig singlePlayer;
	};


	//=========================================================================
	// Factory Methods
	//=========================================================================

	static GameConfiguration createLocalCoop()
	{
		GameConfiguration config{};
		config.mode		 = GameModeSelection::LocalCoop;
		config.localCoop = LocalCoopConfig{};
		return config;
	}

	static GameConfiguration createSinglePlayer(const Side humanColor, const CPUDifficulty difficulty)
	{
		GameConfiguration config{};
		config.mode							 = GameModeSelection::SinglePlayer;
		config.singlePlayer.humanPlayerColor = humanColor;
		config.singlePlayer.aiDifficulty	 = difficulty;
		return config;
	}


	//=========================================================================
	// Accessors
	//=========================================================================

	[[nodiscard]] const SinglePlayerConfig &getSinglePlayer() const
	{
		assert(mode == GameModeSelection::SinglePlayer);
		return singlePlayer;
	}
};
