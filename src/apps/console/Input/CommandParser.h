/*
  ==============================================================================
	Module:         CommandParser
	Description:    Turns a typed line into a Command
  ==============================================================================
*/

#pragma once

#include <optional>
#include <string>

#include "Command.h"


namespace CommandParser
{

/**
 * @brief	Parse one line of console input.
 *
 *			Anything that is not a recognised keyword is treated as a move, so
 *			"e2e4", "e2 e4" and "e7e8q" all work without a command word.
 */
Command				  parse(const std::string &line);

// "e4" -> Square::e4. Case-insensitive.
std::optional<Square> parseSquare(const std::string &text);

// 'q'/'r'/'b'/'n' -> the matching white piece (colour is irrelevant to move lookup).
std::optional<PieceType> parsePromotion(char c);

} // namespace CommandParser
