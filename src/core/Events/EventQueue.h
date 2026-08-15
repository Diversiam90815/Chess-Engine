/*
  ==============================================================================
	Module:         EventQueue
	Description:    Ordered, thread-safe channel carrying EngineEvents to the host
  ==============================================================================
*/

#pragma once

#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>

#include "EngineEvent.h"


/**
 * @brief	Single ordered stream of engine notifications.
 *
 *			Producers (StateMachine, Player, MultiplayerManager) push from whatever
 *			thread they run on; the host drains on its own thread.
 */
class EventQueue
{
public:
	EventQueue()  = default;
	~EventQueue() = default;

	EventQueue(const EventQueue &)			 = delete;
	EventQueue &operator=(const EventQueue &) = delete;

	/**
	 * @brief	Append an event and notify waiters.
	 */
	void		push(engine::EngineEvent event);

	/**
	 * @brief	Take the oldest event if one is pending. Never blocks.
	 * @return	true if an event was written to @p out.
	 */
	bool		poll(engine::EngineEvent &out);

	/**
	 * @brief	Take the oldest event, waiting for one to arrive.
	 * @param	timeout	Zero waits indefinitely (until close()).
	 * @return	true if an event was written to @p out, false on timeout or close.
	 */
	bool		waitAndPop(engine::EngineEvent &out, std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());

	/**
	 * @brief	Install a callback invoked after every push.
	 *			Pass nullptr to remove it.
	 */
	void		setWakeHook(std::function<void()> hook);

	/**
	 * @brief	Wake every blocked waiter and reject further pushes. Used at shutdown.
	 */
	void		close();

	/**
	 * @brief	Drop all pending events without delivering them.
	 */
	void		clear();

	[[nodiscard]] bool	 isClosed() const;
	[[nodiscard]] size_t size() const;


private:
	std::deque<engine::EngineEvent> mQueue;
	mutable std::mutex				mMutex;
	std::condition_variable			mCondition;
	bool							mClosed{false};

	std::function<void()>			mWakeHook;
	mutable std::mutex				mHookMutex;
};
