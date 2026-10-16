/*
  ==============================================================================
	Module:         LogSink
	Description:    Where the engine's log lines go: the host decides
  ==============================================================================
*/

#pragma once

#include <functional>
#include <string>
#include <mutex>


namespace engine
{

enum class LogLevel
{
	Trace	 = 0,
	Debug	 = 1,
	Info	 = 2,
	Warning	 = 3,
	Error	 = 4,
	Critical = 5,
};


struct LogRecord
{
	LogLevel		   level{LogLevel::Info};
	const char		  *file{""};	 // source file of the call
	int				   line{0};
	const char		  *function{""};
	const std::string &message;
};


/**
 * @brief	Receives every log line of the engine at or above the log level.
 *			Called on whatever thread logs; it must not log through the engine itself.
 */
using LogSink = std::function<void(const LogRecord &record)>;


/**
 * @brief	Sets where the log lines go; an empty sink drops them (the default).
 *			Waits until no line is being written to the previous sink, so a host can
 *			remove its sink and then free what it uses.
 */
void setLogSink(LogSink sink);

// Lines below this level are neither formatted nor passed on. Default: Info.
void setLogLevel(LogLevel level);

bool isLogEnabled(LogLevel level);

void writeLog(LogLevel level, const char *file, int line, const char *function, const std::string &message);

} // namespace engine
