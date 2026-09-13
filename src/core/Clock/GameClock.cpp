/*
  ==============================================================================
	Module:         GameClock
	Description:    Owns runtime clock state
  ==============================================================================
*/

#include "GameClock.h"


GameClock::GameClock() : mWorker(&GameClock::workerLoop, this) {}


GameClock::~GameClock()
{
	{
		std::lock_guard<std::mutex> lock(mMutex);

		mStopping = true;
		mRunning  = false;
		++mGeneration;
	}

	notifyWorker();

	if (mWorker.joinable())
		mWorker.join();
}


void GameClock::configure(TimeControl white, TimeControl black)
{
	std::lock_guard<std::mutex> lock(mMutex);

	mWhiteControl	= white;
	mBlackControl	= black;

	mWhiteRemaining = white.isEnabled() ? white.initialTime : Duration::zero();
	mBlackRemaining = black.isEnabled() ? black.initialTime : Duration::zero();

	mRunning		= false;
	mRunningSide	= Side::None;
	++mGeneration;
}


void GameClock::reset()
{
	std::lock_guard<std::mutex> lock(mMutex);

	mWhiteRemaining = mWhiteControl.isEnabled() ? mWhiteControl.initialTime : Duration::zero();
	mBlackRemaining = mBlackControl.isEnabled() ? mBlackControl.initialTime : Duration::zero();

	mRunning		= false;
	mRunningSide	= Side::None;
	++mGeneration; // invalidate any previous timer

	notifyWorker();
}


void GameClock::stop()
{
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mRunning && isPlayerSide(mRunningSide))
		{
			const Duration remaining	   = std::chrono::duration_cast<Duration>(mDeadline - Clock::now());
			remainingForSide(mRunningSide) = clampNonNegative(remaining);
		}

		mRunning	 = false;
		mRunningSide = Side::None;

		++mGeneration;
	}

	notifyWorker();
}


void GameClock::start(Side side)
{
	if (!isPlayerSide(side))
		return;

	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mRunning)
			return;

		if (remainingForSide(side) <= Duration::zero())
			return;

		mRunning	 = true;
		mRunningSide = side;
		mDeadline	 = Clock::now() + remainingForSide(side);

		++mGeneration;
	}

	notifyWorker();
}


void GameClock::pause()
{
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (!mRunning || !isPlayerSide(mRunningSide))
			return;

		const Duration remaining	   = std::chrono::duration_cast<Duration>(mDeadline - Clock::now());
		remainingForSide(mRunningSide) = clampNonNegative(remaining);

		mRunning					   = false;
		mRunningSide				   = Side::None;

		++mGeneration;
	}

	notifyWorker();
}


void GameClock::resume(Side side)
{
	if (!isPlayerSide(side))
		return;

	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mRunning)
			return;

		if (remainingForSide(side) <= Duration::zero())
			return;

		mRunning	 = true;
		mRunningSide = side;
		mDeadline	 = Clock::now() + remainingForSide(side);

		++mGeneration;
	}

	notifyWorker();
}


bool GameClock::completeMove(Side mover)
{
	if (!isPlayerSide(mover))
		return false;

	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (!mRunning || mRunningSide != mover)
			return false;

		const Duration remaining	  = std::chrono::duration_cast<Duration>(mDeadline - Clock::now());

		auto		  &moverRemaining = remainingForSide(mover);
		moverRemaining				  = clampNonNegative(remaining);

		if (moverRemaining <= Duration::zero())
		{
			mRunning	 = false;
			mRunningSide = Side::None;
			++mGeneration;
			notifyWorker();
			return false;
		}

		const TimeControl &control = mover == Side::White ? mWhiteControl : mBlackControl;
		moverRemaining += control.increment;

		mRunning	 = false;
		mRunningSide = Side::None;
		++mGeneration;
	}

	notifyWorker();
	return true;
}


GameClock::Duration GameClock::getRemaining(Side side) const
{
	if (!isPlayerSide(side))
		return Duration::zero();

	std::lock_guard<std::mutex> lock(mMutex);

	Duration					remaining = remainingForSide(side);

	if (mRunning && mRunningSide == side)
		remaining = std::chrono::duration_cast<Duration>(mDeadline - Clock::now());

	return clampNonNegative(remaining);
}


bool GameClock::isRunning() const
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mRunning;
}


Side GameClock::getRunningSide() const
{
	std::lock_guard<std::mutex> lock(mMutex);
	return mRunningSide;
}


bool GameClock::isExpired(Side side) const
{
	return getRemaining(side) <= Duration::zero();
}


ClockSnapshot GameClock::snapshot() const
{
	std::lock_guard<std::mutex> lock(mMutex);

	ClockSnapshot				result;
	result.enabled		  = mWhiteControl.isEnabled() || mBlackControl.isEnabled();

	result.whiteRemaining = mWhiteRemaining;
	result.blackRemaining = mBlackRemaining;

	if (mRunning && mRunningSide == Side::White)
		result.whiteRemaining = clampNonNegative(std::chrono::duration_cast<Duration>(mDeadline - Clock::now()));
	else if (mRunning && mRunningSide == Side::Black)
		result.blackRemaining = clampNonNegative(std::chrono::duration_cast<Duration>(mDeadline - Clock::now()));

	return result;
}


void GameClock::restore(const ClockSnapshot &snapshot)
{
	{
		std::lock_guard<std::mutex> lock(mMutex);

		mWhiteRemaining = clampNonNegative(snapshot.whiteRemaining);
		mBlackRemaining = clampNonNegative(snapshot.blackRemaining);

		mRunning		= false;
		mRunningSide	= Side::None;

		++mGeneration;
	}

	notifyWorker();
}


void GameClock::setTimeoutCallback(TimeoutCallback callback)
{
	std::lock_guard<std::mutex> lock(mMutex);
	mTimeoutCallback = std::move(callback);
}


void GameClock::workerLoop()
{
	std::unique_lock<std::mutex> lock(mMutex);

	while (!mStopping)
	{
		mCondition.wait(lock, [this] { return mStopping || mRunning; });

		if (mStopping)
			break;

		const uint64_t			generation	= mGeneration;
		const Side				side		= mRunningSide;
		const Clock::time_point deadline	= mDeadline;

		const bool				interrupted = mCondition.wait_until(lock, deadline, [this, generation] { return mStopping || !mRunning || mGeneration != generation; });

		if (mStopping)
			break;

		// Stopped, reset, restarted, or otherwise changed
		if (interrupted)
			continue;

		if (!mRunning || mGeneration != generation || mRunningSide != side)
			continue;

		// deadline has been reached
		remainingForSide(side) = Duration::zero();
		mRunning			   = false;
		mRunningSide		   = Side::None;

		++mGeneration;

		TimeoutCallback callback = mTimeoutCallback;

		lock.unlock();

		if (callback)
			callback(side);

		lock.lock();
	}
}


void GameClock::notifyWorker()
{
	mCondition.notify_all();
}


bool GameClock::isPlayerSide(Side side) noexcept
{
	return side == Side::White || side == Side::Black;
}


GameClock::Duration GameClock::clampNonNegative(Duration value) noexcept
{
	return std::max(value, Duration::zero());
}


GameClock::Duration &GameClock::remainingForSide(Side side)
{
	return side == Side::White ? mWhiteRemaining : mBlackRemaining;
}


const GameClock::Duration &GameClock::remainingForSide(Side side) const
{
	return side == Side::White ? mWhiteRemaining : mBlackRemaining;
}
