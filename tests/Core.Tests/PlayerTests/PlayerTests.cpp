/*
  ==============================================================================
	Module:			Player Tests
	Description:    Testing the Player class functionality
  ==============================================================================
*/

#include <gtest/gtest.h>

#include <optional>
#include <vector>

#include "EventQueue.h"
#include "Player/Player.h"


namespace PlayerTests
{

class PlayerTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mEvents		 = std::make_unique<EventQueue>();
		mWhitePlayer = std::make_unique<Player>(Side::White);
		mBlackPlayer = std::make_unique<Player>(Side::Black);

		mWhitePlayer->setEventQueue(mEvents.get());
		mBlackPlayer->setEventQueue(mEvents.get());
	}

	/// Everything the players published since the last call.
	std::vector<engine::PieceCaptured> drainCaptures()
	{
		std::vector<engine::PieceCaptured> captures;
		engine::EngineEvent				   event;

		while (mEvents->poll(event))
		{
			if (const auto *capture = std::get_if<engine::PieceCaptured>(&event))
				captures.push_back(*capture);
		}

		return captures;
	}

	std::unique_ptr<EventQueue> mEvents;
	std::unique_ptr<Player>		mWhitePlayer;
	std::unique_ptr<Player>		mBlackPlayer;
};


//=============================================================================
// CONSTRUCTOR TESTS
//=============================================================================

TEST_F(PlayerTests, DefaultConstructor)
{
	Player player;

	EXPECT_EQ(player.getPlayerColor(), Side::None) << "Default player should have no color";
	EXPECT_TRUE(player.isLocalPlayer()) << "Default player should be local player";
}


TEST_F(PlayerTests, ParameterizedConstructor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "White player should have white color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), Side::Black) << "Black player should have black color";
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "New player should be local by default";
	EXPECT_TRUE(mBlackPlayer->isLocalPlayer()) << "New player should be local by default";
}


//=============================================================================
// PLAYER COLOR TESTS
//=============================================================================

TEST_F(PlayerTests, GetPlayerColor)
{
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "White player should return White color";
	EXPECT_EQ(mBlackPlayer->getPlayerColor(), Side::Black) << "Black player should return Black color";
}


TEST_F(PlayerTests, SetPlayerColor)
{
	Player player;
	player.setPlayerColor(Side::White);

	EXPECT_EQ(player.getPlayerColor(), Side::White) << "Player color should be set to White";

	player.setPlayerColor(Side::Black);
	EXPECT_EQ(player.getPlayerColor(), Side::Black) << "Player color should be changed to Black";
}


TEST_F(PlayerTests, SetPlayerColorToSameValue)
{
	mWhitePlayer->setPlayerColor(Side::White);
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::White) << "Setting same color should work";
}


TEST_F(PlayerTests, SetPlayerColorToNone)
{
	mWhitePlayer->setPlayerColor(Side::None);
	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::None) << "Player color can be set to None";
}


//=============================================================================
// CAPTURED PIECES TESTS
//=============================================================================

TEST_F(PlayerTests, AddCapturedPiece)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 1u) << "One capture should publish one event";
	EXPECT_EQ(captures[0].player, Side::White);
	EXPECT_EQ(captures[0].piece, PieceType::BPawn);
	EXPECT_TRUE(captures[0].captured) << "Adding a piece should report captured = true";

	ASSERT_EQ(mWhitePlayer->getCapturedPieces().size(), 1u);
	EXPECT_EQ(mWhitePlayer->getCapturedPieces().front(), PieceType::BPawn);
}


TEST_F(PlayerTests, AddMultipleCapturedPieces)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 3u) << "Each capture should publish its own event";
	EXPECT_EQ(captures[0].piece, PieceType::BPawn);
	EXPECT_EQ(captures[1].piece, PieceType::BKnight);
	EXPECT_EQ(captures[2].piece, PieceType::BRook) << "Events should arrive in the order they happened";
}


TEST_F(PlayerTests, AddCapturedPieceSameTypeMultipleTimes)
{
	// Should notify each time, even for the same piece type
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);

	EXPECT_EQ(drainCaptures().size(), 3u);
}


TEST_F(PlayerTests, RemoveLastCapturedPiece)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	drainCaptures();

	mWhitePlayer->removeLastCapturedPiece();

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 1u);
	EXPECT_EQ(captures[0].piece, PieceType::BRook) << "The most recent capture should be the one removed";
	EXPECT_FALSE(captures[0].captured) << "Removing a piece should report captured = false";
}


TEST_F(PlayerTests, RemoveLastCapturedPieceFromEmpty)
{
	// Should log a warning but neither crash nor publish anything.
	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece()) << "Removing from empty list should not throw";

	EXPECT_TRUE(drainCaptures().empty()) << "Nothing was removed, so nothing should be published";
}


TEST_F(PlayerTests, RemoveLastCapturedPieceSequence)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	drainCaptures();

	mWhitePlayer->removeLastCapturedPiece();
	mWhitePlayer->removeLastCapturedPiece();
	mWhitePlayer->removeLastCapturedPiece();

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 3u);
	EXPECT_EQ(captures[0].piece, PieceType::BRook);
	EXPECT_EQ(captures[1].piece, PieceType::BKnight);
	EXPECT_EQ(captures[2].piece, PieceType::BPawn) << "Pieces should come off in reverse order";
}


TEST_F(PlayerTests, NoEventQueueIsHarmless)
{
	Player detached(Side::White);

	EXPECT_NO_THROW(detached.addCapturedPiece(PieceType::BPawn)) << "A player without a queue should still work";
	EXPECT_NO_THROW(detached.removeLastCapturedPiece());
	EXPECT_TRUE(detached.getCapturedPieces().empty());
}


//=============================================================================
// RESET FUNCTIONALITY TESTS
//=============================================================================

TEST_F(PlayerTests, Reset)
{
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);

	mWhitePlayer->reset();
	drainCaptures();

	EXPECT_TRUE(mWhitePlayer->getCapturedPieces().empty()) << "Reset should clear the captured pieces";

	EXPECT_NO_THROW(mWhitePlayer->removeLastCapturedPiece()) << "Should not crash when removing from empty list after reset";
	EXPECT_TRUE(drainCaptures().empty()) << "There is nothing left to remove after a reset";
}


TEST_F(PlayerTests, ResetPreservesPlayerColor)
{
	Side originalColor = mWhitePlayer->getPlayerColor();

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->getPlayerColor(), originalColor) << "Player color should be preserved after reset";
}


TEST_F(PlayerTests, ResetPreservesLocalPlayerStatus)
{
	mWhitePlayer->setIsLocalPlayer(false);
	bool originalLocalStatus = mWhitePlayer->isLocalPlayer();

	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->reset();

	EXPECT_EQ(mWhitePlayer->isLocalPlayer(), originalLocalStatus) << "Local player status should be preserved after reset";
}


TEST_F(PlayerTests, ResetAfterMultipleOperations)
{
	mWhitePlayer->setPlayerColor(Side::Black);
	mWhitePlayer->setIsLocalPlayer(false);
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);

	mWhitePlayer->reset();
	drainCaptures();

	EXPECT_EQ(mWhitePlayer->getPlayerColor(), Side::Black) << "Color should be preserved";
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Local status should be preserved";
	EXPECT_TRUE(mWhitePlayer->getCapturedPieces().empty()) << "Captured pieces should be cleared";
}


TEST_F(PlayerTests, ResetMultipleTimes)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->reset();

	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->reset();

	mWhitePlayer->addCapturedPiece(PieceType::BRook);
	mWhitePlayer->reset();
	drainCaptures();

	// Should still publish normally after repeated resets
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);

	const auto captures = drainCaptures();
	ASSERT_EQ(captures.size(), 1u);
	EXPECT_EQ(captures[0].piece, PieceType::BQueen);
}


//=============================================================================
// LOCAL PLAYER TESTS
//=============================================================================

TEST_F(PlayerTests, IsLocalPlayerDefault)
{
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Player should be local by default";
	EXPECT_TRUE(mBlackPlayer->isLocalPlayer()) << "Player should be local by default";
}


TEST_F(PlayerTests, SetIsLocalPlayer)
{
	mWhitePlayer->setIsLocalPlayer(false);
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Player should not be local after setting to false";

	mWhitePlayer->setIsLocalPlayer(true);
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Player should be local after setting to true";
}


TEST_F(PlayerTests, SetIsLocalPlayerMultipleTimes)
{
	mWhitePlayer->setIsLocalPlayer(false);
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Should be remote";

	mWhitePlayer->setIsLocalPlayer(false); // Set to same value
	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Should remain remote";

	mWhitePlayer->setIsLocalPlayer(true);
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Should be local again";

	mWhitePlayer->setIsLocalPlayer(true); // Set to same value
	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "Should remain local";
}


TEST_F(PlayerTests, LocalPlayerStatusIndependentOfOtherProperties)
{
	mWhitePlayer->setIsLocalPlayer(false);
	mWhitePlayer->addCapturedPiece(PieceType::BQueen);
	mWhitePlayer->setPlayerColor(Side::Black);

	EXPECT_FALSE(mWhitePlayer->isLocalPlayer()) << "Local status should be independent of other operations";
}


TEST_F(PlayerTests, LocalPlayerStatusForBothPlayers)
{
	mWhitePlayer->setIsLocalPlayer(true);
	mBlackPlayer->setIsLocalPlayer(false);

	EXPECT_TRUE(mWhitePlayer->isLocalPlayer()) << "White should be local";
	EXPECT_FALSE(mBlackPlayer->isLocalPlayer()) << "Black should be remote";
}


//=============================================================================
// SHARED QUEUE
//=============================================================================

TEST_F(PlayerTests, BothPlayersShareOneOrderedStream)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mBlackPlayer->addCapturedPiece(PieceType::WKnight);
	mWhitePlayer->addCapturedPiece(PieceType::BBishop);

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 3u) << "Both players publish into the same queue";
	EXPECT_EQ(captures[0].player, Side::White);
	EXPECT_EQ(captures[1].player, Side::Black);
	EXPECT_EQ(captures[2].player, Side::White) << "Interleaved captures should keep their order";
}


//=============================================================================
// EDGE CASES
//=============================================================================

TEST_F(PlayerTests, AddCapturedPieceNone)
{
	mWhitePlayer->addCapturedPiece(PieceType::None);

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 1u);
	EXPECT_EQ(captures[0].piece, PieceType::None);
}


TEST_F(PlayerTests, LargeNumberOfCapturedPieces)
{
	for (int i = 0; i < 100; ++i)
	{
		mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	}

	EXPECT_EQ(drainCaptures().size(), 100u);
	EXPECT_EQ(mWhitePlayer->getCapturedPieces().size(), 100u);
}


TEST_F(PlayerTests, AddAndRemoveSequence)
{
	mWhitePlayer->addCapturedPiece(PieceType::BPawn);
	mWhitePlayer->removeLastCapturedPiece();
	mWhitePlayer->addCapturedPiece(PieceType::BKnight);
	mWhitePlayer->removeLastCapturedPiece();

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 4u);
	EXPECT_EQ(captures[0].piece, PieceType::BPawn);
	EXPECT_TRUE(captures[0].captured);
	EXPECT_EQ(captures[1].piece, PieceType::BPawn);
	EXPECT_FALSE(captures[1].captured);
	EXPECT_EQ(captures[2].piece, PieceType::BKnight);
	EXPECT_TRUE(captures[2].captured);
	EXPECT_EQ(captures[3].piece, PieceType::BKnight);
	EXPECT_FALSE(captures[3].captured);
}


TEST_F(PlayerTests, BlackPlayerOperations)
{
	mBlackPlayer->addCapturedPiece(PieceType::WPawn);
	mBlackPlayer->removeLastCapturedPiece();

	const auto captures = drainCaptures();

	ASSERT_EQ(captures.size(), 2u);
	EXPECT_EQ(captures[0].player, Side::Black);
	EXPECT_EQ(captures[0].piece, PieceType::WPawn);
	EXPECT_EQ(captures[1].player, Side::Black);
	EXPECT_FALSE(captures[1].captured);
}

} // namespace PlayerTests
