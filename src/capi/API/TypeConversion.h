/*
  ==============================================================================
	Module:         TypeConversion
	Description:    Marshalling helpers between the C ABI and the C++ core
  ==============================================================================
*/

#pragma once

#include <string>

#include "EngineEvent.h"
#include "EngineSettings.h"
#include "EngineTypes.h"
#include "GameConfiguration.h"


// Copy a std::string into a fixed C buffer, always null-terminating.
void			  CopyStringToBuffer(char *dest, size_t destSize, const std::string &src);

// Flat C game configuration -> the core's tagged GameConfiguration.
GameConfiguration ToGameConfiguration(const CGameConfiguration &config);

// Host startup payload -> the core's EngineSettings.
EngineSettings	  ToEngineSettings(const CUserSettingsInit &settings);

// Core connection event -> its flat C mirror.
CConnectionEvent  ToCConnectionEvent(const engine::ConnectionChanged &event);
