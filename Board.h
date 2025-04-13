#pragma once
#include <vector>
#include <string>
#include <utility>
#include <array>

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
    enum SpecialFlags {
        NORMAL = 0,
        CASTLING_KINGSIDE,
        CASTLING_QUEENSIDE,
        EN_PASSANT,
        PROMOTION
    };

    int fromX, fromY, toX, toY;
    SpecialFlags special;
    
    Move(int frX = -1, int frY = -1, int tX = -1, int tY = -1, SpecialFlags sp = NORMAL);
    bool isValid() const;
    std::string toChessNotation() const;
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
    bool isPathClearForCastling(int row, int startCol, int endCol) const;
    bool isSquareUnderAttack(int x, int y, bool byWhite) const;
    bool isStalemate(bool isWhite) const;

public:
    char board[8][8];

    Board();
    void resetBoard();
    void print() const;
    bool makeMove(const Move& move);
    std::vector<Move> generateAllMoves(bool isWhite) const;
    bool isCheck(bool isWhite) const;
    bool isCheckmate(bool isWhite) const;
};