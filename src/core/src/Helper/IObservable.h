/*
  ==============================================================================
	Module:			IObservable
	Description:    Interface observable class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include <memory>
#include <vector>
#include <algorithm>

#include "IObserver.h"


template <typename ObserverType>
class ObservableBase
{
public:
	virtual void attachObserver(std::weak_ptr<ObserverType> observer) { mObservers.push_back(observer); }
	virtual void detachObserver(std::weak_ptr<ObserverType> observer)
	{
		std::shared_ptr<ObserverType> target = observer.lock();
		if (!target)
			return;

		mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(),
										[&target](const std::weak_ptr<ObserverType> &obs)
										{
											auto current = obs.lock();
											return !current || current == target;
										}),
						 mObservers.end());
	}

protected:
	std::vector<std::weak_ptr<ObserverType>> mObservers;
};


class IPlayerObservable : public ObservableBase<IPlayerObserver>
{
public:
	virtual ~IPlayerObservable() {};

	virtual void addCapturedPiece(const PieceType captured) = 0;
	virtual void removeLastCapturedPiece()					= 0;
};


class IMultiplayerObservable : public ObservableBase<IMultiplayerObserver>
{
public:
	virtual ~IMultiplayerObservable() {};

	virtual void multiplayerStateChanged(const MultiplayerEvent &event) = 0;
	virtual void opponentFound(const std::string &name)					= 0;
	virtual void remotePlayerChosen(Side remotePlayer)					= 0;
};
