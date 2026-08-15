/*
  ==============================================================================
	Module:         CommandParser
	Description:    Turns a typed line into a Command
  ==============================================================================
*/

#include "CommandParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>


namespace CommandParser
{

namespace
{

std::string toLower(std::string text)
{
	std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return text;
}


std::vector<std::string> tokenize(const std::string &line)
{
	std::vector<std::string> tokens;
	std::istringstream		 stream(line);
	std::string				 token;

	while (stream >> token)
		tokens.push_back(token);

	return tokens;
}


Command invalid(std::string reason)
{
	Command command;
	command.type  = CommandType::Invalid;
	command.error = std::move(reason);
	return command;
}


/**
 * @brief	Read a move written as one blob ("e2e4", "e7e8q") or as separate
 *			squares ("e2 e4", "e7 e8 q").
 */
Command parseMove(const std::vector<std::string> &tokens)
{
	std::string joined;
	for (const auto &token : tokens)
		joined += toLower(token);

	if (joined.size() < 4 || joined.size() > 5)
		return invalid("Moves look like 'e2e4' or 'e7e8q'. Type 'help' for the full list.");

	auto from = parseSquare(joined.substr(0, 2));
	auto to	  = parseSquare(joined.substr(2, 2));

	if (!from || !to)
		return invalid("'" + joined + "' is not a pair of squares.");

	Command command;
	command.type = CommandType::Move;
	command.from = *from;
	command.to	 = *to;

	if (joined.size() == 5)
	{
		auto promotion = parsePromotion(joined[4]);

		if (!promotion)
			return invalid("Promote to one of q, r, b or n.");

		command.promotion = *promotion;
	}

	return command;
}

} // namespace


std::optional<Square> parseSquare(const std::string &text)
{
	if (text.size() != 2)
		return std::nullopt;

	const char file = static_cast<char>(std::tolower(static_cast<unsigned char>(text[0])));
	const char rank = text[1];

	if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
		return std::nullopt;

	// Square::a8 is index 0, so rank 8 is the first row.
	const int index = (8 - (rank - '0')) * 8 + (file - 'a');
	return static_cast<Square>(index);
}


std::optional<PieceType> parsePromotion(char c)
{
	switch (std::tolower(static_cast<unsigned char>(c)))
	{
	case 'q': return PieceType::WQueen;
	case 'r': return PieceType::WRook;
	case 'b': return PieceType::WBishop;
	case 'n': return PieceType::WKnight;
	default: return std::nullopt;
	}
}


Command parse(const std::string &line)
{
	const auto tokens = tokenize(line);

	if (tokens.empty())
		return Command{};

	const std::string keyword = toLower(tokens[0]);

	if (keyword == "quit" || keyword == "exit" || keyword == "q")
		return Command{CommandType::Quit};

	if (keyword == "help" || keyword == "?")
		return Command{CommandType::Help};

	if (keyword == "board" || keyword == "b")
		return Command{CommandType::ShowBoard};

	if (keyword == "undo" || keyword == "u")
		return Command{CommandType::Undo};

	if (keyword == "new")
		return Command{CommandType::NewGame};

	if (keyword == "history" || keyword == "h")
		return Command{CommandType::History};

	if (keyword == "flip")
		return Command{CommandType::Flip};

	if (keyword == "moves" || keyword == "m")
	{
		Command command;
		command.type = CommandType::ListMoves;

		if (tokens.size() > 1)
		{
			auto square = parseSquare(toLower(tokens[1]));

			if (!square)
				return invalid("'" + tokens[1] + "' is not a square.");

			command.square = *square;
		}

		return command;
	}

	// Anything else is taken as a move so players can just type coordinates.
	if (keyword == "move")
		return parseMove({tokens.begin() + 1, tokens.end()});

	return parseMove(tokens);
}

} // namespace CommandParser
