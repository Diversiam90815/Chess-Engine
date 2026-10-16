/*
  ==============================================================================
	Module:         EngineLog
	Description:    The log macros the engine is written with
  ==============================================================================
*/

#pragma once

#include <format>

#include "LogSink.h"


// The message is only formatted when the host takes lines of that level
#define ENGINE_LOG(level, fmtStr, ...)                                                                                                                  \
	do                                                                                                                                                  \
	{                                                                                                                                                   \
		if (engine::isLogEnabled(level))                                                                                                                \
			engine::writeLog(level, __FILE__, __LINE__, __FUNCTION__, std::format(fmtStr, ##__VA_ARGS__));                                              \
	} while (false)

#define LOG_TRACE(fmtStr, ...)	  ENGINE_LOG(engine::LogLevel::Trace, fmtStr, ##__VA_ARGS__)
#define LOG_DEBUG(fmtStr, ...)	  ENGINE_LOG(engine::LogLevel::Debug, fmtStr, ##__VA_ARGS__)
#define LOG_INFO(fmtStr, ...)	  ENGINE_LOG(engine::LogLevel::Info, fmtStr, ##__VA_ARGS__)
#define LOG_WARNING(fmtStr, ...)  ENGINE_LOG(engine::LogLevel::Warning, fmtStr, ##__VA_ARGS__)
#define LOG_ERROR(fmtStr, ...)	  ENGINE_LOG(engine::LogLevel::Error, fmtStr, ##__VA_ARGS__)
#define LOG_CRITICAL(fmtStr, ...) ENGINE_LOG(engine::LogLevel::Critical, fmtStr, ##__VA_ARGS__)
