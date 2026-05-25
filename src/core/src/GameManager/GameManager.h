/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include "GameController.h"
#include "Player.h"
#include "Logging.h"
#include "IObservable.h"
#include "WinUIInputSource.h"
#include "MultiplayerManager.h"
#include "PlayerName.h"
#include "SystemInfo.h"
#include "StateMachine.h"


class GameManager
{
public:
	~GameManager();

	static GameManager					*GetInstance();
	static void							 ReleaseInstance();

	//=========================================================================
	// Lifecycle
	//=========================================================================

	bool								 init();
	void								 shutDown();

	//=========================================================================
	// Game Control
	//=========================================================================

	void								 startGame(GameConfiguration config);
	void								 resetGame();
	void								 undoMove();

	//=========================================================================
	// Input Events
	//=========================================================================

	void								 onSquareSelected(Square sq);
	void								 onPromotionChosen(PieceType piece);

	//=========================================================================
	// UI Integration
	//=========================================================================

	void								 setDelegate(PFN_CALLBACK delegate);

	//=========================================================================
	// Board State Queries
	//=========================================================================

	std::array<PieceType, 64>			 getBoardPieces() const;
	const MoveList						&getCachedLegalMoves() const;
	PieceType							 getPieceAt(Square sq) const;

	//=========================================================================
	// Multiplayer
	//=========================================================================

	void								 startMultiplayer();
	void								 stopMultiplayer();
	bool								 isMultiplayerActive() const { return mIsMultiplayerMode; }

	void								 findOpponent();
	std::vector<std::string>			 getDiscoveredOpponents();
	void								 connectToOpponent(int index);
	void								 respondToConnectionRequest(bool accept);

	void								 setLocalPlayerColor(Side localPlayer);
	void								 setLocalPlayerReady(bool ready);

	//=========================================================================
	// Network Adapters
	//=========================================================================

	std::vector<netlink::NetworkAdapter> getNetworkAdapters();
	bool								 changeCurrentNetworkAdapter(int ID);


private:
	GameManager() = default;

	void								initializeComponents();
	void								wireComponents();

	//=========================================================================
	// Core Components
	//=========================================================================

	std::unique_ptr<GameController>		mGameController;
	std::unique_ptr<StateMachine>		mStateMachine;
	std::shared_ptr<WinUIInputSource>	mInputSource;

	//=========================================================================
	// Infrastructure
	//=========================================================================

	Logging								mLog;
	PlayerName							mPlayerName;

	std::shared_ptr<MultiplayerManager> mMultiplayerManager;

	//=========================================================================
	// State
	//=========================================================================

	bool								mIsMultiplayerMode{false};
	bool								mInitialized{false};
};
