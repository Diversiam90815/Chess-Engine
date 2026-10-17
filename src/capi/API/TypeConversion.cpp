/*
  ==============================================================================
	Module:         TypeConversion
	Description:    Marshalling helpers between the C ABI and the C++ core
  ==============================================================================
*/
#include <cstring>

#include "TypeConversion.h"
#include "EngineLog.h"


void CopyStringToBuffer(char *dest, const size_t destSize, const std::string &src)
{
	if (!dest || destSize == 0)
		return;

	const size_t len = src.size() < destSize - 1 ? src.size() : destSize - 1;

	std::memcpy(dest, src.data(), len);
	dest[len] = '\0';
}


GameConfiguration ToGameConfiguration(const CGameConfiguration &config)
{
	const auto mode		  = static_cast<GameModeSelection>(config.mode);
	const auto playerSide = static_cast<Side>(config.playerColor);
	const auto difficulty = static_cast<CPUDifficulty>(config.cpuDifficulty);

	switch (mode)
	{
	case GameModeSelection::LocalCoop: return GameConfiguration::createLocalCoop();
	case GameModeSelection::SinglePlayer: return GameConfiguration::createSinglePlayer(playerSide, difficulty);
	default: LOG_ERROR("Invalid game mode received from C API: {}", config.mode); break;
	}

	// fallback: local coop
	return GameConfiguration::createLocalCoop();
}
