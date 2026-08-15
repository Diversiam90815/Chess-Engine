/*
  ==============================================================================
	Module:         ConsoleApp
	Description:    Interactive console front end for the chess engine
  ==============================================================================
*/

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "Command.h"
#include "GameManager.h"
#include "GameView.h"


class ConsoleApp
{
public:
	ConsoleApp();
	~ConsoleApp();

	ConsoleApp(const ConsoleApp &)			  = delete;
	ConsoleApp &operator=(const ConsoleApp &) = delete;

	// Run until the player quits or input ends. Returns a process exit code.
	int			run();


private:
	//=========================================================================
	// Setup
	//=========================================================================

	static void				 configureConsole();
	static EngineSettings	 defaultSettings();

	bool					 startNewGame();

	//=========================================================================
	// Event Pump
	//=========================================================================

	/**
	 * @brief	Wait for the engine to react, then keep consuming until it goes quiet.
	 * @param	firstWait	How long to wait for the first event.
	 */
	void					 pumpUntilIdle(std::chrono::milliseconds firstWait);

	void					 handleEvent(const engine::EngineEvent &event);

	//=========================================================================
	// Turn Handling
	//=========================================================================

	[[nodiscard]] bool		 canPrompt() const;

	void					 promptPromotion();
	void					 promptCommand();
	void					 dispatch(const Command &command);

	void					 submitMove(const Command &command);
	
	[[nodiscard]] bool		 resolveSan(const std::string &san, Square &from, Square &to, PieceType &promotion) const;

	bool					 readLine(const char *prompt, std::string &line) const;

	//=========================================================================
	// State
	//=========================================================================

	GameManager				 mEngine;
	GameView				 mView;

	/// Mirrors the engine's state machine, updated from GameStateChanged events.
	GameState				 mState{GameState::Init};

	/// Mode of the game in progress; decides how far 'undo' has to reach back.
	GameModeSelection		 mMode{GameModeSelection::LocalCoop};

	/// Side that made the move currently being reported.
	Side					 mMover{Side::White};

	/// SAN of every move played, so 'history' can show real notation.
	std::vector<std::string> mMoveLog;

	bool					 mRunning{true};
	bool					 mBoardDirty{true};
};
