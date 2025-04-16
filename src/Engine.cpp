#include "../include/Engine.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <limits>

// Просто дерьмово работает

Move ChessEngine::findBestMove(Board& board, bool isWhite, int depth) {
    std::vector<Move> moves = board.generateAllMoves(isWhite);
    if (moves.empty()) return Move(-1, -1, -1, -1);

    Move bestMove(-1, -1, -1, -1);
    int bestValue = std::numeric_limits<int>::min();
    
    std::vector<Move> orderedMoves = orderMoves(board, moves);
    
    for (const Move& move : orderedMoves) {
        Board tempBoard = board;
        tempBoard.makeMove(move);
        
        int moveValue = minimax(tempBoard, depth-1, 
                              std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max(),
                              !isWhite);
        
        if (moveValue > bestValue) {
            bestValue = moveValue;
            bestMove = move;
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
    
    const int pawnTable[8][8] = {
        {0,  0,  0,  0,  0,  0,  0,  0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {5,  5, 10, 25, 25, 10,  5,  5},
        {0,  0,  0, 20, 20,  0,  0,  0},
        {5, -5,-10,  0,  0,-10, -5,  5},
        {5, 10, 10,-20,-20, 10, 10,  5},
        {0,  0,  0,  0,  0,  0,  0,  0}
    };
    
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            char piece = board.board[i][j];
            score += pieceValues.at(piece);
            
            if (piece == 'P') score += pawnTable[i][j];
            else if (piece == 'p') score -= pawnTable[7-i][j];
        }
    }
    
    if (board.isCheck(true)) score -= 50;
    if (board.isCheck(false)) score += 50;
    
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
            score += 100;
        }
        
        scoredMoves.emplace_back(score, move);
    }
    
    std::sort(scoredMoves.rbegin(), scoredMoves.rend(), 
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<Move> result;
    for (const auto& [score, move] : scoredMoves) {
        result.push_back(move);
    }
    
    return result;
}