/*
  ==============================================================================
	Module:         GameView
	Description:    All console rendering for the chess app
  ==============================================================================
*/

#include "GameView.h"

#include <cstdio>

#include "BoardPrinter.h"
#include "MovePrinter.h"


namespace
{

const char *squareName(Square square)
{
	const int index = to_index(square);
	return (index >= 0 && index < 64) ? square_to_coordinates[index] : "--";
}

} // namespace


void GameView::printBanner() const
{
	printf("\n");
	printf("=============================================\n");
	printf("            Chess Engine - Console           \n");
	printf("=============================================\n");
}


void GameView::printBoard() const
{
	BoardPrinter::Options options;
	options.flip	   = mFlip;
	options.unicode	   = mUnicode;
	options.showStatus = false;

	BoardPrinter::printBoard(mEngine.getBoard(), options);
}


void GameView::printStatus() const
{
	const Side side = mEngine.getCurrentSide();

	printf("   %s to move", sideName(side).c_str());

	if (mEngine.isInCheck())
		printf("  -  CHECK");

	printf("\n\n");
}


void GameView::printLegalMoves(Square from) const
{
	MoveList moves;
	mEngine.getLegalMovesFromSquare(from, moves);

	if (moves.empty())
	{
		printf("  No legal moves from %s.\n\n", squareName(from));
		return;
	}

	BoardPrinter::Options options;
	options.flip	   = mFlip;
	options.unicode	   = mUnicode;
	options.showStatus = false;

	for (const Move &move : moves)
		options.highlight.push_back(move.to());

	BoardPrinter::printBoard(mEngine.getBoard(), options);

	printf("  %zu move%s from %s:\n", moves.size(), moves.size() == 1 ? "" : "s", squareName(from));

	std::vector<std::string> labels;
	labels.reserve(moves.size());
	for (const Move &move : moves)
		labels.push_back(mEngine.getMoveNotation(move));

	MovePrinter::printMoveTable(labels);
	printf("\n");
}


void GameView::printAllLegalMoves() const
{
	const MoveList			&moves = mEngine.getLegalMoves();

	std::vector<std::string> labels;
	labels.reserve(moves.size());
	for (const Move &move : moves)
		labels.push_back(mEngine.getMoveNotation(move));

	printf("  %zu legal move%s for %s:\n", moves.size(), moves.size() == 1 ? "" : "s", sideName(mEngine.getCurrentSide()).c_str());
	MovePrinter::printMoveTable(labels);
	printf("\n");
}


void GameView::printHistory(const std::vector<std::string> &sanLog) const
{
	if (sanLog.empty())
	{
		printf("  No moves played yet.\n\n");
		return;
	}

	printf("  Move history:\n");

	for (size_t i = 0; i < sanLog.size(); i += 2)
	{
		printf("  %3zu. %-10s", (i / 2) + 1, sanLog[i].c_str());

		if (i + 1 < sanLog.size())
			printf("%-10s", sanLog[i + 1].c_str());

		printf("\n");
	}

	printf("\n");
}


void GameView::printHelp() const
{
	printf("\n");
	printf("  Commands\n");
	printf("  --------\n");
	printf("    e2e4 / e2 e4    Play a move\n");
	printf("    e7e8q           Play a move, promoting to q, r, b or n\n");
	printf("    moves           List every legal move\n");
	printf("    moves e2        List the moves leaving one square\n");
	printf("    board           Re-draw the board\n");
	printf("    undo            Take back the last move\n");
	printf("    history         Show the moves played so far\n");
	printf("    flip            Switch the board orientation\n");
	printf("    new             Start a new game\n");
	printf("    help            Show this list\n");
	printf("    quit            Leave\n");
	printf("\n");
}


void GameView::printMovePlayed(const engine::MoveExecuted &event, Side mover) const
{
	printf("  %s plays %s\n", sideName(mover).c_str(), event.notation.c_str());
}


void GameView::printCapture(const engine::PieceCaptured &event) const
{
	if (!event.captured)
		return;

	printf("  %s captures a %s\n", sideName(event.player).c_str(), pieceName(event.piece).c_str());
}


void GameView::printGameEnded(const engine::GameEnded &event) const
{
	printf("\n  ---------------------------------\n");

	switch (event.state)
	{
	case EndGameState::Checkmate: printf("   Checkmate - %s wins!\n", sideName(event.winner).c_str()); break;

	case EndGameState::StaleMate: printf("   Stalemate - the game is drawn.\n"); break;

	case EndGameState::Draw:
	{
		switch (event.reason)
		{
		case DrawReason::FiftyMoveRule: printf("   Draw by the fifty-move rule.\n"); break;
		case DrawReason::InsufficientMaterial: printf("   Draw - insufficient material.\n"); break;
		case DrawReason::ThreefoldRepetition: printf("   Draw by threefold repetition.\n"); break;
		default: printf("   The game is drawn.\n"); break;
		}
		break;
	}

	default: printf("   Game over.\n"); break;
	}

	printf("  ---------------------------------\n");
	printf("  Type 'new' to play again, or 'quit' to leave.\n\n");
}


void GameView::printMessage(const std::string &text) const
{
	printf("  %s\n", text.c_str());
}


void GameView::printError(const std::string &text) const
{
	printf("  ! %s\n", text.c_str());
}


std::string GameView::sideName(Side side)
{
	switch (side)
	{
	case Side::White: return "White";
	case Side::Black: return "Black";
	default: return "Nobody";
	}
}


std::string GameView::pieceName(PieceType piece)
{
	switch (piece)
	{
	case WKing:
	case BKing: return "king";
	case WQueen:
	case BQueen: return "queen";
	case WRook:
	case BRook: return "rook";
	case WBishop:
	case BBishop: return "bishop";
	case WKnight:
	case BKnight: return "knight";
	case WPawn:
	case BPawn: return "pawn";
	default: return "piece";
	}
}
