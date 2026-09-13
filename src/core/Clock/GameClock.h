/*
  ==============================================================================
	Module:         GameClock
	Description:    Owns runtime clock state
  ==============================================================================
*/

#pragma once

#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>

#include "TimeControl.h"
#include "ClockSnapshot.h"
#include "BitboardTypes.h"



class GameClock
{
public:
	using Clock			  = std::chrono::steady_clock;
	using Duration		  = std::chrono::milliseconds;
	using TimeoutCallback = std::function<void(Side)>;

	GameClock();
	~GameClock();
	GameClock(const GameClock &)			   = delete;
	GameClock	 &operator=(const GameClock &) = delete;

	// Configuration & Lifecycle
	void		  configure(TimeControl white, TimeControl black);
	void		  reset();
	void		  stop();

	// Turn control
	void		  start(Side side);
	void		  pause();
	void		  resume(Side side);

	// Must be called after a valid move has been accepted!
	bool		  completeMove(Side mover);

	// Queries
	Duration	  getRemaining(Side side) const;
	bool		  isRunning() const;
	Side		  getRunningSide() const;
	bool		  isExpired(Side side) const;

	// Undo support
	ClockSnapshot snapshot() const;
	void		  restore(const ClockSnapshot &snapshot);

	void		  setTimeoutCallback(TimeoutCallback callback);

private:
	void					workerLoop();
	void					notifyWorker();

	static bool				isPlayerSide(Side side) noexcept;
	static Duration			clampNonNegative(Duration value) noexcept;

	Duration			   &remainingForSide(Side side);
	const Duration		   &remainingForSide(Side side) const;


	mutable std::mutex		mMutex;
	std::condition_variable mCondition;
	std::thread				mWorker;

	bool					mStopping{false};
	bool					mRunning{false};

	Side					mRunningSide{Side::None};

	TimeControl				mWhiteControl{};
	TimeControl				mBlackControl{};

	Duration				mWhiteRemaining{0};
	Duration				mBlackRemaining{0};

	Clock::time_point		mDeadline{};
	uint64_t				mGeneration{0};

	TimeoutCallback			mTimeoutCallback;
};
