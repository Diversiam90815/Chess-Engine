/*
  ==============================================================================
	Module:         GameManager
	Description:    Manager for the Chess game
  ==============================================================================
*/

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "EngineSettings.h"
#include "EventQueue.h"
#include "GameController.h"
#include "Logging.h"
#include "MultiplayerManager.h"
#include "Player.h"
#include "StateMachine.h"
#include "SystemInfo.h"


/**
 * @brief	Facade over the engine: commands go in, EngineEvents come out.
 */
class GameManager
{
public:
	GameManager() = default;
	~GameManager();
	GameManager(const GameManager &)									= delete;
	GameManager							&operator=(const GameManager &) = delete;

	//=========================================================================
	// Lifecycle
	//=========================================================================

	bool								 init(EngineSettings settings);
	void								 shutDown();
	[[nodiscard]] bool					 isInitialized() const { return mInitialized; }

	//=========================================================================
	// Host Notification Channel
	//=========================================================================

	EventQueue							&events() { return mEvents; }
	const EventQueue					&events() const { return mEvents; }

	//=========================================================================
	// Game Control
	//=========================================================================

	void								 startGame(GameConfiguration config);
	void								 resetGame();
	void								 undoMove();

	//=========================================================================
	// Input Events
	//=========================================================================

	/**
	 * @brief	Submit a fully specified move. Preferred by hosts that already know
	 *			both squares (console, tests, engine protocols).
	 *			Leave @p promotion as None to be asked via a PromotionRequired event.
	 */
	void								 submitMove(Square from, Square to, PieceType promotion = PieceType::None);

	/// Click-driven path: first call selects, second call targets.
	void								 onSquareSelected(Square sq);
	void								 onPromotionChosen(PieceType piece);

	//=========================================================================
	// Board & Game State Queries
	//=========================================================================

	[[nodiscard]] GameState				 getGameState() const;
	[[nodiscard]] Side					 getCurrentSide() const;
	[[nodiscard]] bool					 isInCheck() const;

	std::array<PieceType, 64>			 getBoardPieces() const;
	PieceType							 getPieceAt(Square sq) const;
	const Chessboard					&getBoard() const;

	/// Every legal move in the current position.
	const MoveList						&getLegalMoves() const;

	/// Legal moves leaving a single square. Pure query - does not change engine state.
	void								 getLegalMovesFromSquare(Square from, MoveList &out) const;

	/// Legal moves of the most recently selected square (drives UI highlighting).
	const MoveList						&getSelectionMoves() const;

	std::string							 getMoveNotation(Move move) const;
	const std::vector<MoveHistoryEntry> &getMoveHistory() const;

	const std::vector<PieceType>		&getCapturedPieces(Side player) const;

	//=========================================================================
	// Settings
	//=========================================================================

	const EngineSettings				&settings() const { return mSettings; }
	void								 setLocalPlayerName(const std::string &name) { mSettings.playerName = name; }
	void								 setDiscoveryPort(int port) { mSettings.discoveryPort = port; }

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
	int									 getCurrentNetworkAdapterID();


private:
	void								ensureMultiplayerManager();

	//=========================================================================
	// Core Components
	//=========================================================================

	EventQueue							mEvents;
	std::unique_ptr<GameController>		mGameController;
	std::unique_ptr<StateMachine>		mStateMachine;

	//=========================================================================
	// Infrastructure
	//=========================================================================

	Logging								mLog;
	EngineSettings						mSettings;

	/// Created on first use so hosts that never go online do not spin up NetLink.
	std::unique_ptr<MultiplayerManager> mMultiplayerManager;

	//=========================================================================
	// State
	//=========================================================================

	bool								mIsMultiplayerMode{false};
	bool								mInitialized{false};
};
