/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include "Move.h"
#include "Parameters.h"
#include "MultiplayerTypes.h"
#include "BitboardTypes.h"


class IPlayerObserver
{
public:
	virtual ~IPlayerObserver() {};

	virtual void onAddCapturedPiece(Side player, PieceType captured)		= 0;
	virtual void onRemoveLastCapturedPiece(Side player, PieceType captured) = 0;
};


class IMultiplayerObserver
{
public:
	virtual ~IMultiplayerObserver() {};

	virtual void onMultiplayerStateChanged(const MultiplayerEvent &event) = 0;
	virtual void onOpponentFound(const std::string &name)				  = 0;
	virtual void onRemotePlayerChosen(Side remotePlayer)				  = 0;
};
