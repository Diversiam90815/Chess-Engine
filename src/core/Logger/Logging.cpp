/*
  ==============================================================================
	Module:         Logger
	Description:    Logging instance for the project
  ==============================================================================
*/


#include "Logging.h"


void Logging::initLogging(const fs::path &loggingFolder)
{
	std::string fileName = getNewLogFilePath(loggingFolder);

	logging::addFileOutput()
		.setFilename(fileName)
		.setLevel(LogLevel::Info)
		.setMaxFileSize(10_MB)
		.setMaxFiles(10)
		.setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS))
		.setRotateOnSession(true);

	logging::addMSVCOutput().checkForPresentDebugger(true).setLevel(LogLevel::Debug).setMaxSkipDuration(std::chrono::microseconds(mSlowLogTimeMS));
}


std::string Logging::gameStateToString(GameState state)
{
	switch (state)
	{
	case GameState::Init: return "Init";
	case GameState::WaitingForInput: return "Waiting For Input";
	case GameState::WaitingForTarget: return "Waiting For Target";
	case GameState::GameOver: return "Game Over";
	case GameState::PawnPromotion: return "Pawn Promotion";
	case GameState::WaitingForRemoteMove: return "Waiting for remote move";
	case GameState::WaitingForCPUMove: return "Waiting for CPU move";
	default: return "Unknown GameState";
	}
}


std::string Logging::boolToString(const bool value)
{
	return value ? "True" : "False";
}


std::string Logging::multiplayerStateToString(MultiplayerState state)
{
	switch (state)
	{
	case MultiplayerState::None: return "None";
	case MultiplayerState::Searching: return "Searching";
	case MultiplayerState::OpponentFound: return "Opponent Found";
	case MultiplayerState::ConnectionRequested: return "Connection Requested";
	case MultiplayerState::Connected: return "Connected";
	case MultiplayerState::PlayerSetup: return "Player Setup";
	case MultiplayerState::ReadyToStart: return "Ready To Start";
	case MultiplayerState::InGame: return "In Game";
	case MultiplayerState::Disconnected: return "Disconnected";
	case MultiplayerState::Error: return "Error";
	default: return "Unknown MultiplayerState";
	}
}


std::string Logging::sideToString(const Side side)
{
	switch (side)
	{
	case Side::None: return "None";
	case Side::White: return "White";
	case Side::Black: return "Black";
	case Side::Both: return "Both";
	default: return "Unknown Side";
	}
}


std::string Logging::cpuDifficultyToString(const CPUDifficulty diff)
{
	switch (diff)
	{
	case CPUDifficulty::Easy: return "Easy";
	case CPUDifficulty::Medium: return "Medium";
	case CPUDifficulty::Hard: return "Hard";
	default: return "Unknown Difficulty";
	}
}


std::string Logging::drawReasonToString(const DrawReason reason)
{
	switch (reason)
	{
	case DrawReason::FiftyMoveRule: return "Fifty-move rule";
	case DrawReason::InsufficientMaterial: return "Insufficient material";
	case DrawReason::ThreefoldRepetition: return "Threefold repetition";
	default: return "None";
	}
}


std::string Logging::getNewLogFilePath(fs::path logFolder)
{
	std::error_code ec;
	fs::path		root = logFolder.empty() ? fs::current_path(ec) : logFolder;
	fs::path		path = root / FileName::LoggingFolder;

	if (!fs::exists(path) && !fs::create_directories(path, ec))
		return "";

	auto		log		 = path / FileName::LogFile;
	std::string fileName = log.string();

	return fileName;
}
