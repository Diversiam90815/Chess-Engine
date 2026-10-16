/*
  ==============================================================================
	Module:         LogSink
	Description:    Where the engine's log lines go: the host decides
  ==============================================================================
*/

#include "LogSink.h"

#include <atomic>
#include <shared_mutex>


namespace
{

std::shared_mutex	  gSinkMutex;
engine::LogSink		  gSink;
std::atomic<bool>	  gHasSink{false};
std::atomic<int>	  gLevel{static_cast<int>(engine::LogLevel::Info)};

} // namespace


void engine::setLogSink(LogSink sink)
{
	// Exclusive: a line that is being written to the old sink finishes first
	std::unique_lock lock(gSinkMutex);
	gSink = std::move(sink);
	gHasSink.store(static_cast<bool>(gSink));
}


void engine::setLogLevel(LogLevel level)
{
	gLevel.store(static_cast<int>(level));
}


bool engine::isLogEnabled(LogLevel level)
{
	return gHasSink.load(std::memory_order_relaxed) && static_cast<int>(level) >= gLevel.load(std::memory_order_relaxed);
}


void engine::writeLog(LogLevel level, const char *file, int line, const char *function, const std::string &message)
{
	std::shared_lock lock(gSinkMutex);

	if (!gSink)
		return;

	try
	{
		gSink(LogRecord{level, file ? file : "", line, function ? function : "", message});
	}
	catch (...)
	{
		// Logging must never take the engine down
	}
}
