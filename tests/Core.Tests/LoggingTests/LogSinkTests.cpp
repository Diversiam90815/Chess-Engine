/*
  ==============================================================================
	Module:			Log Sink Tests
	Description:    Testing how the engine hands its log lines to the host
  ==============================================================================
*/

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "EngineLog.h"


namespace LoggingTests
{

struct Line
{
	engine::LogLevel level;
	std::string		 file;
	int				 line;
	std::string		 function;
	std::string		 message;
};


class LogSinkTest : public ::testing::Test
{
protected:
	std::vector<Line> mLines;

	void			  SetUp() override
	{
		engine::setLogLevel(engine::LogLevel::Info);
		engine::setLogSink([this](const engine::LogRecord &record) { mLines.push_back({record.level, record.file, record.line, record.function, record.message}); });
	}

	void TearDown() override
	{
		engine::setLogSink({});
		engine::setLogLevel(engine::LogLevel::Info);
	}
};


TEST_F(LogSinkTest, Line_ReachesTheSinkWithItsSource)
{
	constexpr int expectedLine = __LINE__ + 1;
	LOG_INFO("Move {} of {}", 3, "White");

	ASSERT_EQ(mLines.size(), 1u);
	EXPECT_EQ(mLines[0].level, engine::LogLevel::Info);
	EXPECT_EQ(mLines[0].message, "Move 3 of White");
	EXPECT_EQ(mLines[0].line, expectedLine);
	EXPECT_NE(mLines[0].file.find("LogSinkTests.cpp"), std::string::npos);
	EXPECT_NE(mLines[0].function.find("TestBody"), std::string::npos);
}


TEST_F(LogSinkTest, EveryMacro_HasItsLevel)
{
	engine::setLogLevel(engine::LogLevel::Trace);

	LOG_TRACE("t");
	LOG_DEBUG("d");
	LOG_INFO("i");
	LOG_WARNING("w");
	LOG_ERROR("e");
	LOG_CRITICAL("c");

	ASSERT_EQ(mLines.size(), 6u);
	EXPECT_EQ(mLines[0].level, engine::LogLevel::Trace);
	EXPECT_EQ(mLines[1].level, engine::LogLevel::Debug);
	EXPECT_EQ(mLines[2].level, engine::LogLevel::Info);
	EXPECT_EQ(mLines[3].level, engine::LogLevel::Warning);
	EXPECT_EQ(mLines[4].level, engine::LogLevel::Error);
	EXPECT_EQ(mLines[5].level, engine::LogLevel::Critical);
}


TEST_F(LogSinkTest, LinesBelowTheLevel_AreNotEvenFormatted)
{
	int	 formatted = 0;
	auto counted   = [&formatted]
	{
		++formatted;
		return 1;
	};

	engine::setLogLevel(engine::LogLevel::Warning);

	LOG_INFO("dropped {}", counted());
	LOG_WARNING("kept {}", counted());

	ASSERT_EQ(mLines.size(), 1u);
	EXPECT_EQ(mLines[0].message, "kept 1");
	EXPECT_EQ(formatted, 1);
}


TEST_F(LogSinkTest, WithoutASink_NothingIsFormattedOrPassedOn)
{
	int	 formatted = 0;
	auto counted   = [&formatted]
	{
		++formatted;
		return 1;
	};

	engine::setLogSink({});
	LOG_ERROR("dropped {}", counted());

	EXPECT_TRUE(mLines.empty());
	EXPECT_EQ(formatted, 0);
	EXPECT_FALSE(engine::isLogEnabled(engine::LogLevel::Critical));
}


TEST_F(LogSinkTest, ThrowingSink_DoesNotReachTheEngine)
{
	engine::setLogSink([](const engine::LogRecord &) { throw std::runtime_error("host failed"); });

	EXPECT_NO_THROW(LOG_ERROR("still running"));
}


TEST_F(LogSinkTest, ReplacingTheSink_WaitsForALineInProgress)
{
	std::atomic<bool> inSink{false};
	std::atomic<bool> release{false};
	std::atomic<bool> finished{false};

	engine::setLogSink(
		[&](const engine::LogRecord &)
		{
			inSink = true;
			while (!release)
				std::this_thread::yield();
			finished = true;
		});

	std::thread writer([] { LOG_INFO("slow"); });

	while (!inSink)
		std::this_thread::yield();

	std::thread remover([] { engine::setLogSink({}); });

	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_FALSE(finished) << "the line is still being written";

	release = true;
	remover.join();
	writer.join();

	EXPECT_TRUE(finished) << "the sink was only removed after the line was done";
}

} // namespace LoggingTests
