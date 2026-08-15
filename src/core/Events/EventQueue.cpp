/*
  ==============================================================================
	Module:         EventQueue
	Description:    Ordered, thread-safe channel carrying EngineEvents to the host
  ==============================================================================
*/

#include "EventQueue.h"


void EventQueue::push(engine::EngineEvent event)
{
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mClosed)
			return;

		mQueue.push_back(std::move(event));
	}

	mCondition.notify_one();

	// Copy the hook out before calling it: the hook typically drains the queue,
	// so it must not run while we hold any of our locks.
	std::function<void()> hook;
	{
		std::lock_guard<std::mutex> lock(mHookMutex);
		hook = mWakeHook;
	}

	if (hook)
		hook();
}


bool EventQueue::poll(engine::EngineEvent &out)
{
	std::lock_guard<std::mutex> lock(mMutex);

	if (mQueue.empty())
		return false;

	out = std::move(mQueue.front());
	mQueue.pop_front();
	return true;
}


bool EventQueue::waitAndPop(engine::EngineEvent &out, std::chrono::milliseconds timeout)
{
	std::unique_lock<std::mutex> lock(mMutex);

	const auto					 ready = [this] { return !mQueue.empty() || mClosed; };

	if (timeout == std::chrono::milliseconds::zero())
		mCondition.wait(lock, ready);
	else if (!mCondition.wait_for(lock, timeout, ready))
		return false;

	if (mQueue.empty())
		return false; // closed while waiting

	out = std::move(mQueue.front());
	mQueue.pop_front();
	return true;
}


void EventQueue::setWakeHook(std::function<void()> hook)
{
	std::lock_guard<std::mutex> lock(mHookMutex);
	mWakeHook = std::move(hook);
}


void EventQueue::close()
{
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mClosed = true;
	}

	mCondition.notify_all();
}


void EventQueue::clear()
{
	std::lock_guard<std::mutex> lock(mMutex);
	mQueue.clear();
}


bool EventQueue::isClosed() const
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mClosed;
}


size_t EventQueue::size() const
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mQueue.size();
}
