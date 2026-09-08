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

	mCondition.notify_one();

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
	++mGeneration;

	mCondition.notify_all();
}


void GameClock::stop() {}


void GameClock::start(Side sie) {}


void GameClock::pause() {}


void GameClock::resume(Side side) {}


bool GameClock::completeMove(Side mover)
{
	return false;
}


GameClock::Duration GameClock::getRemaining(Side side) const
{
	return Duration();
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
	return ClockSnapshot();
}


void GameClock::restore(const ClockSnapshot &snapshot) {}


void GameClock::setTimeoutCallback(TimeoutCallback callback)
{
	std::lock_guard<std::mutex> lock(mMutex);
	mTimeoutCallback = std::move(callback);
}


void GameClock::workerLoop() {}


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
