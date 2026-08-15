/*
  ==============================================================================
	Module:         EngineInstance
	Description:    Process-wide engine instance backing the C ABI
  ==============================================================================
*/

#include "EngineInstance.h"

#include <memory>
#include <mutex>


namespace
{

struct Instance
{
	GameManager	   engine;
	CallbackBridge bridge;
};

std::unique_ptr<Instance> gInstance;
std::mutex				  gInstanceMutex;

} // namespace


namespace EngineInstance
{

void Startup(EngineSettings settings)
{
	std::lock_guard<std::mutex> lock(gInstanceMutex);

	if (gInstance)
		return;

	auto instance = std::make_unique<Instance>();

	instance->engine.init(std::move(settings));

	// Attach after init so the bridge sees the queue the engine actually publishes to.
	instance->bridge.attach(&instance->engine.events());

	gInstance = std::move(instance);
}


void Shutdown()
{
	std::unique_ptr<Instance> doomed;

	{
		std::lock_guard<std::mutex> lock(gInstanceMutex);
		doomed = std::move(gInstance);
	}

	if (!doomed)
		return;

	doomed->bridge.attach(nullptr);
	doomed->engine.shutDown();
}


GameManager *Engine()
{
	std::lock_guard<std::mutex> lock(gInstanceMutex);
	return gInstance ? &gInstance->engine : nullptr;
}


CallbackBridge *Bridge()
{
	std::lock_guard<std::mutex> lock(gInstanceMutex);
	return gInstance ? &gInstance->bridge : nullptr;
}

} // namespace EngineInstance
