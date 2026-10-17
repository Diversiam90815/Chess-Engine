/*
  ==============================================================================
	Module:         ConsoleLogging
	Description:    Writes the engine's log lines to a log file of the console app
  ==============================================================================
*/

#include "ConsoleLogging.h"

#include <cstdlib>

// Only the sink API of the engine: its LOG_* macros would clash with the Logger library's
#include "LogSink.h"
#include "Logger.h"


namespace
{

constexpr auto LoggingFolder = "Logs";
constexpr auto LogFile		 = "Chess.log";
constexpr auto SlowLogTime	 = std::chrono::microseconds(500);


LogLevel	   toLoggerLevel(const engine::LogLevel level)
{
	switch (level)
	{
	case engine::LogLevel::Trace: return LogLevel::Trace;
	case engine::LogLevel::Debug: return LogLevel::Debug;
	case engine::LogLevel::Info: return LogLevel::Info;
	case engine::LogLevel::Warning: return LogLevel::Warn;
	case engine::LogLevel::Error: return LogLevel::Error;
	default: return LogLevel::Critical;
	}
}

} // namespace


void ConsoleLogging::enable(const std::filesystem::path &folder)
{
	std::error_code				ec;
	const std::filesystem::path logFolder = (folder.empty() ? std::filesystem::current_path(ec) : folder) / LoggingFolder;

	if (!std::filesystem::exists(logFolder) && !std::filesystem::create_directories(logFolder, ec))
		return;

	logging::addFileOutput()
		.setFilename((logFolder / LogFile).string())
		.setLevel(LogLevel::Info)
		.setMaxFileSize(10_MB)
		.setMaxFiles(10)
		.setMaxSkipDuration(SlowLogTime)
		.setRotateOnSession(true);

	logging::addMSVCOutput().checkForPresentDebugger(true).setLevel(LogLevel::Debug).setMaxSkipDuration(SlowLogTime);

	engine::setLogSink([](const engine::LogRecord &record) { logging::log(toLoggerLevel(record.level), record.file, record.line, record.function, record.message); });
}


void ConsoleLogging::disable()
{
	engine::setLogSink({});
}


std::filesystem::path ConsoleLogging::defaultFolder()
{
#if defined(_WIN32)
	if (const char *localAppData = std::getenv("LOCALAPPDATA"))
		return std::filesystem::path(localAppData) / "ChessEngine";
#else
	if (const char *home = std::getenv("HOME"))
		return std::filesystem::path(home) / ".chessengine";
#endif

	return {};
}
