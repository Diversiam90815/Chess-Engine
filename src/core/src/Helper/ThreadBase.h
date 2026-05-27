/*
  ==============================================================================
	Module:         ThreadBase
	Description:    Base class for thread management with event triggering
  ==============================================================================
*/

#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>


class ThreadBase
{
public:
	ThreadBase()		  = default;
	virtual ~ThreadBase() = default;

	/**
	 * @brief	Starts the worker thread if not already running.
	 *			Calls the derived run() method on a new std::thread.
	 *			Subsequent calls while running are ignored.
	 */
	void start()
	{
		if (isRunning())
			return;

		mRunning.store(true);
		mThread = std::thread(&ThreadBase::run, this);
	}

	/**
	 * @brief	Requests the thread to stop and joins it.
	 *
	 * Sets the running flag to false, signals the condition variable to wake
	 * a blocked waitForEvent(), then joins the thread.
	 *
	 * Safe to call multiple times; only the first effective call performs work.
	 *
	 * @note	Must NOT be invoked from inside the worker thread (would deadlock).
	 */
	virtual void stop()
	{
		if (!isRunning())
			return;

		mRunning.store(false);
		triggerEvent(); // Wake up the thread

		if (mThread.joinable())
			mThread.join();
	}

	/**
	 * @brief	Triggers an event waking at most one thread waiting in waitForEvent().
	 *			Sets an internal flag consumed by waitForEvent(). If multiple triggers
	 *			happen before the waiter wakes, they coalesce into a single observed event.
	 */
	void triggerEvent()
	{
		{
			std::lock_guard<std::mutex> lock(mMutex);
			mEventTriggered = true;
		}
		cv.notify_one();
	}

	/**
	 * @brief	Indicates whether the worker thread is currently marked as running.
	 * @return	true if running flag is set, false otherwise.
	 */
	bool isRunning() const { return mRunning.load(); }


protected:
	/**
	 * @brief	Main execution function executed in the spawned thread context.
	 *
	 * Derived classes must implement a cooperative loop!
	 */
	virtual void run() = 0;

	/**
	 * @brief	Blocks until an event is triggered or the thread is asked to stop.
	 *
	 * @param	timeoutMS Optional timeout in milliseconds. 0 (default) means wait indefinitely.
	 *
	 * Behavior:
	 *  - Returns true only if an event was signaled (triggerEvent()) AND the thread
	 *    is still in the running state.
	 *  - Returns false if:
	 *      * Timeout elapsed without an event, or
	 *      * A stop() request occurred (isRunning() == false).
	 *
	 * The internal event flag is automatically reset (consumed) before returning.
	 *
	 * @note	Multiple triggerEvent() calls before the wait unblocks are coalesced.
	 * @return	true if an event was consumed while still running; false otherwise.
	 */
	bool		 waitForEvent(unsigned long timeoutMS = 0)
	{
		std::unique_lock<std::mutex> lock(mMutex);

		if (timeoutMS > 0)
		{
			cv.wait_for(lock, std::chrono::milliseconds(timeoutMS), [this] { return mEventTriggered || !isRunning(); });
		}
		else
		{
			cv.wait(lock, [this] { return mEventTriggered || !isRunning(); });
		}
		bool wasTriggered = mEventTriggered;
		mEventTriggered	  = false;			// Reset the flag

		return wasTriggered && isRunning(); // Return true if event was triggered and thread is still running
	}


private:
	std::thread				mThread;				///< Worker thread instance
	std::atomic<bool>		mRunning;				///< Running state flag (set by start()/stop() )
	std::mutex				mMutex;					///< Protect event flag
	std::condition_variable cv;						///< Condition variable for event signaling
	bool					mEventTriggered{false}; ///< Indicates an event has been triggerd
};