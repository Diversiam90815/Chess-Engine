/*
  ==============================================================================
	Module:         EngineInstance
	Description:    Process-wide engine instance backing the C ABI
  ==============================================================================
*/

#pragma once

#include "CallbackBridge.h"
#include "GameManager.h"


/**
 * @brief	Owns the one GameManager the C API talks to.
 */
namespace EngineInstance
{

// Create and initialize the engine. Safe to call twice.
void			Startup(EngineSettings settings);

// Tear the engine down and drop the instance.
void			Shutdown();

// The live engine, or nullptr before Startup()/after Shutdown().
GameManager	   *Engine();

// The callback bridge, or nullptr before Startup()/after Shutdown().
CallbackBridge *Bridge();

} // namespace EngineInstance
