/*
  ==============================================================================
	Module:         GameSetup
	Description:    Interactive prompts that build a GameConfiguration
  ==============================================================================
*/

#include "GameSetup.h"

#include <cstdio>
#include <iostream>
#include <random>
#include <string>


namespace GameSetup
{

namespace
{

/**
 * @brief	Prompt until the player picks one of @p optionCount numbered choices.
 * @return	The 1-based choice, or nullopt on end of input.
 */
std::optional<int> askChoice(const char *prompt, int optionCount)
{
	std::string line;

	while (true)
	{
		printf("%s", prompt);
		fflush(stdout);

		if (!std::getline(std::cin, line))
			return std::nullopt;

		try
		{
			const int choice = std::stoi(line);

			if (choice >= 1 && choice <= optionCount)
				return choice;
		}
		catch (const std::exception &)
		{
			// fall through to the reprompt
		}

		printf("  Please enter a number between 1 and %d.\n", optionCount);
	}
}


Side randomSide()
{
	std::random_device				rd;
	std::mt19937					gen(rd());
	std::uniform_int_distribution<> dist(0, 1);

	return dist(gen) == 0 ? Side::White : Side::Black;
}

} // namespace


std::optional<GameConfiguration> run()
{
	printf("\n  How would you like to play?\n");
	printf("    1) Local co-op  (two players, one board)\n");
	printf("    2) Against the computer\n\n");

	const auto mode = askChoice("  Mode [1-2]: ", 2);

	if (!mode)
		return std::nullopt;

	if (*mode == 1)
	{
		printf("\n  Local co-op it is. White starts.\n");
		return GameConfiguration::createLocalCoop();
	}

	printf("\n  Which colour do you want?\n");
	printf("    1) White\n");
	printf("    2) Black\n");
	printf("    3) Random\n\n");

	const auto colour = askChoice("  Colour [1-3]: ", 3);

	if (!colour)
		return std::nullopt;

	Side playerSide = Side::White;

	if (*colour == 2)
		playerSide = Side::Black;
	else if (*colour == 3)
		playerSide = randomSide();

	printf("\n  How strong should the computer be?\n");
	printf("    1) Easy\n");
	printf("    2) Medium\n");
	printf("    3) Hard\n\n");

	const auto strength = askChoice("  Difficulty [1-3]: ", 3);

	if (!strength)
		return std::nullopt;

	const CPUDifficulty difficulty = static_cast<CPUDifficulty>(*strength);

	printf("\n  You play %s against the computer.\n", playerSide == Side::White ? "White" : "Black");

	return GameConfiguration::createSinglePlayer(playerSide, difficulty);
}

} // namespace GameSetup
