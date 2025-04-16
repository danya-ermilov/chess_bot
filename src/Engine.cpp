#include "../include/Engine.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <limits>

Move ChessEngine::findBestMove(Board& board, bool isWhite, int depth) {
    std::vector<Move> moves = board.generateAllMoves(isWhite);
    if (moves.empty()) return Move(-1, -1, -1, -1);

    Move bestMove = moves[0]; // По умолчанию первый ход
    int bestValue = isWhite ? std::numeric_limits<int>::min() 
                           : std::numeric_limits<int>::max();
    
    std::vector<Move> orderedMoves = orderMoves(board, moves);
    
    for (const Move& move : orderedMoves) {
        Board tempBoard = board;
        if (!tempBoard.makeMove(move)) continue;
        
        int moveValue = minimax(tempBoard, depth-1, 
                              std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max(),
                              !isWhite);
        
        if (isWhite) {
            if (moveValue > bestValue) {
                bestValue = moveValue;
                bestMove = move;
            }
        } else {
            if (moveValue < bestValue) {
                bestValue = moveValue;
                bestMove = move;
            }
        }
    }

    return bestMove;
}

int ChessEngine::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || board.isCheckmate(!maximizingPlayer) || board.isStalemate(!maximizingPlayer)) {
        return evaluateBoard(board);
    }
    
    std::vector<Move> moves = board.generateAllMoves(maximizingPlayer);
    moves = orderMoves(board, moves);
    
    if (maximizingPlayer) {
        int maxEval = std::numeric_limits<int>::min();
        for (const Move& move : moves) {
            Board tempBoard = board;
            tempBoard.makeMove(move);
            int eval = minimax(tempBoard, depth-1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const Move& move : moves) {
            Board tempBoard = board;
            tempBoard.makeMove(move);
            int eval = minimax(tempBoard, depth-1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

int ChessEngine::evaluateBoard(const Board& board) {
    int score = 0;
    
    const std::unordered_map<char, int> pieceValues = {
        {'P', 100}, {'N', 320}, {'B', 330}, {'R', 500}, {'Q', 900}, {'K', 20000},
        {'p', -100}, {'n', -320}, {'b', -330}, {'r', -500}, {'q', -900}, {'k', -20000}, {'.', 0}
    };
    
    const int centerControl[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 3, 2, 1, 0},
        {0, 2, 4, 6, 6, 4, 2, 0},
        {0, 3, 6, 9, 9, 6, 3, 0},
        {0, 3, 6, 9, 9, 6, 3, 0},
        {0, 2, 4, 6, 6, 4, 2, 0},
        {0, 1, 2, 3, 3, 2, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };
    
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            char piece = board.board[i][j];
            score += pieceValues.at(piece) / 10;
            
            if (isupper(piece)) score += centerControl[i][j];
            else if (islower(piece)) score -= centerControl[i][j];
        }
    }
    
    if (board.isCheck(true)) score -= 50;
    if (board.isCheck(false)) score += 50;
    
    int whiteMobility = board.generateAllMoves(true).size();
    int blackMobility = board.generateAllMoves(false).size();
    score += (whiteMobility - blackMobility);
    
    return score;
}

std::vector<Move> ChessEngine::orderMoves(const Board& board, const std::vector<Move>& moves) {
    std::vector<std::pair<int, Move>> scoredMoves;
    
    for (const Move& move : moves) {
        int score = 0;
        char targetPiece = board.board[move.toX][move.toY];
        
        // Приоритет взятий (MVV-LVA)
        if (targetPiece != EMPTY) {
            const std::unordered_map<char, int> pieceValues = {
                {'P', 100}, {'N', 320}, {'B', 330}, {'R', 500}, {'Q', 900}, {'K', 20000},
                {'p', 100}, {'n', 320}, {'b', 330}, {'r', 500}, {'q', 900}, {'k', 20000}, {'.', 0}
            };
            char movingPiece = board.board[move.fromX][move.fromY];
            score = 10 * pieceValues.at(targetPiece) - pieceValues.at(movingPiece);
        }
        
        Board tempBoard = board;
        tempBoard.makeMove(move);
        if (tempBoard.isCheck(!board.isWhite(move.fromX, move.fromY))) {
            score += 1000;
        }

        scoredMoves.emplace_back(score, move);
    }
    
    std::sort(scoredMoves.rbegin(), scoredMoves.rend(), 
        [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<Move> result;
    for (const auto& [score, move] : scoredMoves) {
        result.push_back(move);
    }
    
    return result;
}