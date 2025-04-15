#pragma once
#include <vector>
#include <string>
#include <utility>
#include <array>
#include <iostream>

enum Piece : char {
    EMPTY = '.',

    PAWN = 'P',
    KNIGHT = 'N',
    BISHOP = 'B',
    ROOK = 'R',
    QUEEN = 'Q',
    KING = 'K',
    
    BLACK_PAWN = 'p',
    BLACK_KNIGHT = 'n',
    BLACK_BISHOP = 'b',
    BLACK_ROOK = 'r',
    BLACK_QUEEN = 'q',
    BLACK_KING = 'k'
};

struct Move {
    // enum SpecialFlags {
    //     NORMAL = 0,
    //     CASTLING_KINGSIDE,
    //     CASTLING_QUEENSIDE,
    //     EN_PASSANT,
    //     PROMOTION
    // };

    int fromX, fromY, toX, toY;
    // SpecialFlags special;

    Move(int frX = -1, int frY = -1, int tX = -1, int tY = -1);
    bool isValid() const;
    static Move fromChessNotation(const std::string& from, const std::string& to);
    std::string toChessNotation() const;

    bool operator==(const Move& other) const {
        return fromX == other.fromX && fromY == other.fromY &&
               toX == other.toX && toY == other.toY;
    }

    friend std::ostream& operator<<(std::ostream& os, const Move& move) {
        
                return os << char('a' + move.fromY) << (8 - move.fromX) 
                         << " " << char('a' + move.toY) << (8 - move.toX);
        
    }
};

class Board {
private:
    void generatePawnMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;
    void generateKnightMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;
    void generateBishopMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;
    void generateRookMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;
    void generateQueenMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;
    void generateKingMoves(int x, int y, bool isWhite, std::vector<Move>& moves) const;

    bool isInBounds(int x, int y) const;
    bool isEmpty(int x, int y) const;
    bool isEnemy(int x, int y, bool isWhite) const;
    bool isAlly(int x, int y, bool isWhite) const;
    bool isWhite(int x, int y) const;

    bool castlingRights[2][2]; // [white/black][kingside/queenside]
    bool kingHasMoved[2];      // [white/black]

    bool hasKingMoved(bool isWhite) const;
    bool canCastleKingside(bool isWhite) const;
    bool canCastleQueenside(bool isWhite) const;
    bool isPathClearForCastling(int y, int startX, int endX) const;
    bool isSquareUnderAttack(int x, int y, bool byWhite) const;
    bool canCastle(bool isWhite, bool kingside) const;

public:
    char board[8][8];

    Board();
    void resetBoard();
    void print() const;
    bool makeMove(const Move& move);
    std::vector<Move> generateAllMoves(bool isWhite) const;
    bool isCheck(bool isWhite) const;
    bool isStalemate(bool isWhite) const;
    bool isCheckmate(bool isWhite) const;
    bool isValidMove(const Move& move, bool isWhiteTurn) const;
};