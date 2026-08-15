/*
  ==============================================================================
	Module:         Command
	Description:    A parsed console instruction
  ==============================================================================
*/

#pragma once

#include <string>

#include "BitboardTypes.h"


enum class CommandType
{
	None = 0,  // Blank line
	Move,	   // Play from -> to (optionally promoting)
	ListMoves, // All legal moves, or those leaving `square`
	ShowBoard, // Re-render
	Undo,
	NewGame,
	History,
	Flip,
	Help,
	Quit,
	Invalid // Could not be parsed; `error` explains why
};


struct Command
{
	CommandType type	  = CommandType::None;

	Square		from	  = Square::None;
	Square		to		  = Square::None;
	PieceType	promotion = PieceType::None;

	Square		square	  = Square::None;
	std::string SAN;

	std::string error;
};
