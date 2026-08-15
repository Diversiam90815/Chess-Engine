/*
  ==============================================================================
	Module:			EventQueue Tests
	Description:    Testing the engine's host notification channel
  ==============================================================================
*/

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "EventQueue.h"


namespace EventQueueTests
{

using namespace std::chrono_literals;


class EventQueueTests : public ::testing::Test
{
protected:
	EventQueue mQueue;
};


//=============================================================================
// POLLING
//=============================================================================

TEST_F(EventQueueTests, PollOnEmptyQueueReturnsFalse)
{
	engine::EngineEvent event;

	EXPECT_FALSE(mQueue.poll(event)) << "An empty queue has nothing to hand out";
	EXPECT_EQ(mQueue.size(), 0u);
}


TEST_F(EventQueueTests, PushThenPoll)
{
	mQueue.push(engine::PlayerChanged{Side::Black});

	engine::EngineEvent event;
	ASSERT_TRUE(mQueue.poll(event));

	const auto *changed = std::get_if<engine::PlayerChanged>(&event);
	ASSERT_NE(changed, nullptr) << "The event should keep its type through the queue";
	EXPECT_EQ(changed->side, Side::Black);

	EXPECT_FALSE(mQueue.poll(event)) << "Polling should consume the event";
}


TEST_F(EventQueueTests, EventsComeOutInOrder)
{
	mQueue.push(engine::GameStateChanged{GameState::WaitingForInput});
	mQueue.push(engine::MoveExecuted{Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush), "e4"});
	mQueue.push(engine::BoardStateChanged{});
	mQueue.push(engine::PlayerChanged{Side::Black});

	ASSERT_EQ(mQueue.size(), 4u);

	engine::EngineEvent event;

	ASSERT_TRUE(mQueue.poll(event));
	EXPECT_TRUE(std::holds_alternative<engine::GameStateChanged>(event));

	ASSERT_TRUE(mQueue.poll(event));
	ASSERT_TRUE(std::holds_alternative<engine::MoveExecuted>(event));
	EXPECT_EQ(std::get<engine::MoveExecuted>(event).notation, "e4") << "Payloads should survive the round trip";

	ASSERT_TRUE(mQueue.poll(event));
	EXPECT_TRUE(std::holds_alternative<engine::BoardStateChanged>(event));

	ASSERT_TRUE(mQueue.poll(event));
	EXPECT_TRUE(std::holds_alternative<engine::PlayerChanged>(event));

	EXPECT_FALSE(mQueue.poll(event));
}


//=============================================================================
// BLOCKING
//=============================================================================

TEST_F(EventQueueTests, WaitAndPopTimesOutOnEmptyQueue)
{
	engine::EngineEvent event;

	const auto			start   = std::chrono::steady_clock::now();
	const bool			gotOne  = mQueue.waitAndPop(event, 50ms);
	const auto			elapsed = std::chrono::steady_clock::now() - start;

	EXPECT_FALSE(gotOne) << "Nothing was pushed, so the wait should time out";
	EXPECT_GE(elapsed, 40ms) << "The wait should actually have waited";
}


TEST_F(EventQueueTests, WaitAndPopReturnsAlreadyQueuedEvent)
{
	mQueue.push(engine::MoveUndone{});

	engine::EngineEvent event;
	ASSERT_TRUE(mQueue.waitAndPop(event, 50ms)) << "A pending event should be returned without waiting";
	EXPECT_TRUE(std::holds_alternative<engine::MoveUndone>(event));
}


TEST_F(EventQueueTests, WaitAndPopWakesOnPushFromAnotherThread)
{
	std::thread producer(
		[this]
		{
			std::this_thread::sleep_for(30ms);
			mQueue.push(engine::BoardStateChanged{});
		});

	engine::EngineEvent event;
	const bool			gotOne = mQueue.waitAndPop(event, 2000ms);

	producer.join();

	EXPECT_TRUE(gotOne) << "A push from a producer thread should wake the waiter";
	EXPECT_TRUE(std::holds_alternative<engine::BoardStateChanged>(event));
}


TEST_F(EventQueueTests, CloseUnblocksWaiter)
{
	std::atomic<bool> finished{false};

	std::thread		  consumer(
		 [this, &finished]
		 {
			 engine::EngineEvent event;
			 mQueue.waitAndPop(event); // indefinite wait
			 finished.store(true);
		 });

	std::this_thread::sleep_for(30ms);
	EXPECT_FALSE(finished.load()) << "The consumer should still be blocked";

	mQueue.close();
	consumer.join();

	EXPECT_TRUE(finished.load()) << "close() must release an indefinite waiter";
	EXPECT_TRUE(mQueue.isClosed());
}


TEST_F(EventQueueTests, PushAfterCloseIsIgnored)
{
	mQueue.close();
	mQueue.push(engine::BoardStateChanged{});

	engine::EngineEvent event;
	EXPECT_FALSE(mQueue.poll(event)) << "A closed queue should accept nothing further";
	EXPECT_EQ(mQueue.size(), 0u);
}


//=============================================================================
// WAKE HOOK
//=============================================================================

TEST_F(EventQueueTests, WakeHookFiresOnEveryPush)
{
	int count = 0;
	mQueue.setWakeHook([&count] { ++count; });

	mQueue.push(engine::MoveUndone{});
	mQueue.push(engine::BoardStateChanged{});

	EXPECT_EQ(count, 2) << "The hook should run once per push";
}


TEST_F(EventQueueTests, WakeHookCanDrainWithoutDeadlocking)
{
	// This is exactly what the C API's CallbackBridge does.
	std::vector<engine::EngineEvent> received;

	mQueue.setWakeHook(
		[this, &received]
		{
			engine::EngineEvent event;
			while (mQueue.poll(event))
				received.push_back(event);
		});

	mQueue.push(engine::PlayerChanged{Side::White});
	mQueue.push(engine::PlayerChanged{Side::Black});

	ASSERT_EQ(received.size(), 2u) << "A draining hook should see every event";
	EXPECT_EQ(std::get<engine::PlayerChanged>(received[0]).side, Side::White);
	EXPECT_EQ(std::get<engine::PlayerChanged>(received[1]).side, Side::Black);
	EXPECT_EQ(mQueue.size(), 0u);
}


TEST_F(EventQueueTests, WakeHookCanBeRemoved)
{
	int count = 0;
	mQueue.setWakeHook([&count] { ++count; });

	mQueue.push(engine::MoveUndone{});
	mQueue.setWakeHook(nullptr);
	mQueue.push(engine::MoveUndone{});

	EXPECT_EQ(count, 1) << "Clearing the hook should stop the callbacks";
}


//=============================================================================
// CLEAR
//=============================================================================

TEST_F(EventQueueTests, ClearDropsPendingEvents)
{
	mQueue.push(engine::MoveUndone{});
	mQueue.push(engine::BoardStateChanged{});
	ASSERT_EQ(mQueue.size(), 2u);

	mQueue.clear();

	engine::EngineEvent event;
	EXPECT_EQ(mQueue.size(), 0u);
	EXPECT_FALSE(mQueue.poll(event));
}

} // namespace EventQueueTests
