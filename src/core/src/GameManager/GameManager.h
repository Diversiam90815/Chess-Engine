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
#include "NetworkManager.h"
#include "MultiplayerManager.h"
#include "PlayerName.h"
#include "SystemInfo.h"
#include "StateMachine.h"


class GameManager
{
public:
	~GameManager();

	static GameManager		   *GetInstance();
	static void					ReleaseInstance();

	//=========================================================================
	// Lifecycle
	//=========================================================================

	bool						init();
	void						shutDown();

	//=========================================================================
	// Game Control
	//=========================================================================

	void						startGame(GameConfiguration config);
	void						resetGame();
	void						undoMove();

	//=========================================================================
	// Input Events
	//=========================================================================

	void						onSquareSelected(Square sq);
	void						onPromotionChosen(PieceType piece);

	//=========================================================================
	// UI Integration
	//=========================================================================

	void						setDelegate(PFN_CALLBACK delegate);

	//=========================================================================
	// Board State Queries
	//=========================================================================

	std::array<PieceType, 64>	getBoardPieces() const;
	const MoveList			   &getCachedLegalMoves() const;
	PieceType					getPieceAt(Square sq) const;

	//=========================================================================
	// Multiplayer
	//=========================================================================

	void						startMultiplayerSession();
	void						stoppedMultiplayer();
	bool						isMultiplayerActive() const { return mIsMultiplayerMode; }

	void						startRemoteDiscovery(bool isHost);
	void						answerConnectionInvitation(bool accepted);
	void						sendConnectionRequestToHost();
	void						setLocalPlayerInMultiplayer(Side localPlayer);
	void						setLocalPlayerReady(bool ready);

	//=========================================================================
	// Network
	//=========================================================================

	std::vector<NetworkAdapter> getNetworkAdapters();
	bool						changeCurrentNetworkAdapter(int ID);
	int							getCurrentNetworkAdapterID();


private:
	GameManager() = default;

	void								initializeComponents();
	void								wireComponents();
	void								setupMultiplayObservers();

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
	std::unique_ptr<NetworkManager>		mNetworkManager;

	//=========================================================================
	// State
	//=========================================================================

	bool								mIsMultiplayerMode{false};
	bool								mInitialized{false};
};
