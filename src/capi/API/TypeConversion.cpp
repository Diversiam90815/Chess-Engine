/*
  ==============================================================================
	Module:         TypeConversion
	Description:    Marshalling helpers between the C ABI and the C++ core
  ==============================================================================
*/

#include "TypeConversion.h"

#include <cstring>

#include "EngineLog.h"


void CopyStringToBuffer(char *dest, size_t destSize, const std::string &src)
{
	if (!dest || destSize == 0)
		return;

	size_t len = src.size() < destSize - 1 ? src.size() : destSize - 1;

	std::memcpy(dest, src.data(), len);
	dest[len] = '\0';
}


GameConfiguration ToGameConfiguration(const CGameConfiguration &config)
{
	GameModeSelection mode		 = static_cast<GameModeSelection>(config.mode);
	Side			  playerSide = static_cast<Side>(config.playerColor);
	CPUDifficulty	  difficulty = static_cast<CPUDifficulty>(config.cpuDifficulty);

	switch (mode)
	{
	case GameModeSelection::LocalCoop: return GameConfiguration::createLocalCoop();
	case GameModeSelection::SinglePlayer: return GameConfiguration::createSinglePlayer(playerSide, difficulty);
	default: LOG_ERROR("Invalid game mode received from C API: {}", config.mode); break;
	}

	// fallback: local coop
	return GameConfiguration::createLocalCoop();
}

