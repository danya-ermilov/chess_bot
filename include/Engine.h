#pragma once
#include "Board.h"
#include <vector>
#include <limits>

class ChessEngine {
public:
    Move findBestMove(Board& board, bool isWhite, int depth);

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);
    int evaluateBoard(const Board& board);
    std::vector<Move> orderMoves(const Board& board, const std::vector<Move>& moves);
};