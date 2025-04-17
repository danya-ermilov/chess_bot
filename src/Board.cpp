#include "../include/Board.h"
#include <algorithm>
#include <cstring>
#include <iostream>

// Доделать рокировку под шахом и взятие пешки на проходе

Move::Move(int frX, int frY, int tX, int tY)
    : fromX(frX), fromY(frY), toX(tX), toY(tY)
{
}

bool Move::isValid() const
{
    return fromX >= 0 && fromX < 8 && fromY >= 0 && fromY < 8 && toX >= 0 &&
           toX < 8 && toY >= 0 && toY < 8;
}

Move Move::fromChessNotation(const std::string &from, const std::string &to)
{
    if (from.length() != 2 || to.length() != 2) {
        throw std::invalid_argument("Некорректный формат хода");
    }

    return Move(8 - (from[1] - '0'), // row (1-8 -> 0-7)
                from[0] - 'a',       // col (a-h -> 0-7)
                8 - (to[1] - '0'),   // row
                to[0] - 'a'          // col
    );
}

std::string Move::toChessNotation() const
{
    return std::string(1, 'a' + fromY) + std::to_string(8 - fromX) + " " +
           std::string(1, 'a' + toY) + std::to_string(8 - toX);
}

Board::Board()
{
    resetBoard();
    memset(kingHasMoved, 0, sizeof(kingHasMoved));
    memset(castlingRights, 1, sizeof(castlingRights));
}

bool Board::isValidMove(const Move &move, bool isWhiteTurn) const
{
    if (!isInBounds(move.fromX, move.fromY) ||
        isWhite(move.fromX, move.fromY) != isWhiteTurn) {
        return false;
    }

    auto possibleMoves = generateAllMoves(isWhiteTurn);

    return std::any_of(possibleMoves.begin(),
                       possibleMoves.end(),
                       [&move](const Move &m) { return m == move; });
}

void Board::resetBoard()
{
    char initialBoard[8][8] = {
        {'r',   'n',   'b',   'q',   'k',   'b',   'n',   'r'  },
        {'p',   'p',   'p',   'p',   'p',   'p',   'p',   'p'  },
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
        {'P',   'P',   'P',   'P',   'P',   'P',   'P',   'P'  },
        {'R',   'N',   'B',   'Q',   'K',   'B',   'N',   'R'  }
    };
    memcpy(board, initialBoard, sizeof(board));
}

void Board::print() const
{
    std::cout << "  a b c d e f g h\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << 8 - i << " ";
        for (int j = 0; j < 8; ++j) {
            std::cout << board[i][j] << " ";
        }
        std::cout << 8 - i << "\n";
    }
    std::cout << "  a b c d e f g h\n";
}

bool Board::isWhite(int x, int y) const
{
    if (!isInBounds(x, y)) {
        return false;
    }
    return isupper(board[x][y]);
}

bool Board::makeMove(const Move &move)
{
    if (!move.isValid() || isEmpty(move.fromX, move.fromY))
        return false;

    char movingPiece = board[move.fromX][move.fromY];
    bool isWhiteMove = isupper(movingPiece);

    Board tempBoard = *this;

    tempBoard.board[move.toX][move.toY] = movingPiece;
    tempBoard.board[move.fromX][move.fromY] = EMPTY;

    if (tempBoard.isCheck(isWhiteMove)) {
        return false;
    }

    if (tolower(movingPiece) == 'k' && abs(move.fromY - move.toY) == 2) {
        if (move.toY > move.fromY) {
            if (!canCastleKingside(isWhiteMove))
                return false;
            tempBoard.board[move.fromX][5] = tempBoard.board[move.fromX][7];
            tempBoard.board[move.fromX][7] = EMPTY;
        } else {
            if (!canCastleQueenside(isWhiteMove))
                return false;
            tempBoard.board[move.fromX][3] = tempBoard.board[move.fromX][0];
            tempBoard.board[move.fromX][0] = EMPTY;
        }
    }

    if (tolower(movingPiece) == 'p' && (move.toX == 0 || move.toX == 7)) {
        tempBoard.board[move.toX][move.toY] =
            isWhiteMove ? 'Q' : 'q'; // Автоматически в ферзя
    }

    *this = tempBoard;

    if (tolower(movingPiece) == 'k') {
        kingHasMoved[isWhiteMove ? 0 : 1] = true;
    }
    if (tolower(movingPiece) == 'r') {
        if (move.fromY == 0)
            castlingRights[isWhiteMove ? 0 : 1][1] = false;
        else if (move.fromY == 7)
            castlingRights[isWhiteMove ? 0 : 1][0] = false;
    }

    return true;
}

bool Board::canCastle(bool isWhite, bool kingside) const
{
    if (hasKingMoved(isWhite))
        return false;

    int row = isWhite ? 7 : 0;
    int rookCol = kingside ? 7 : 0;

    char expectedRook = isWhite ? 'R' : 'r';
    if (board[row][rookCol] != expectedRook)
        return false;

    int start = kingside ? 4 : 0;
    int end = kingside ? 7 : 4;
    int step = kingside ? 1 : -1;

    for (int col = start + step; col != end; col += step) {
        if (board[row][col] != EMPTY)
            return false;
        if (isSquareUnderAttack(row, col, !isWhite))
            return false;
    }

    return true;
}

bool Board::isInBounds(int x, int y) const
{
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

bool Board::isEmpty(int x, int y) const
{
    return isInBounds(x, y) && board[x][y] == EMPTY;
}

bool Board::isEnemy(int x, int y, bool isWhite) const
{
    if (!isInBounds(x, y))
        return false;
    char piece = board[x][y];
    return isWhite ? islower(piece) : isupper(piece);
}

bool Board::isAlly(int x, int y, bool isWhite) const
{
    if (!isInBounds(x, y))
        return false;
    char piece = board[x][y];
    return isWhite ? isupper(piece) : islower(piece);
}

bool Board::isCheck(bool isWhite) const
{
    char king = isWhite ? 'K' : 'k';
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board[x][y] == king) {
                return isSquareUnderAttack(x, y, !isWhite);
            }
        }
    }
    return false;
}

bool Board::isCheckmate(bool isWhite) const
{
    if (!isCheck(isWhite))
        return false;

    auto moves = generateAllMoves(isWhite);
    for (const Move &move : moves) {
        Board tempBoard = *this;
        if (tempBoard.makeMove(move)) {
            if (!tempBoard.isCheck(isWhite)) {
                return false;
            }
        }
    }

    return true;
}

bool Board::hasKingMoved(bool isWhite) const
{
    return kingHasMoved[isWhite ? 0 : 1];
}

bool Board::canCastleKingside(bool isWhite) const
{
    const int y = isWhite ? 7 : 0;
    return castlingRights[isWhite ? 0 : 1][0] &&
           isPathClearForCastling(y, 4, 7) && !isCheck(isWhite);
}

bool Board::canCastleQueenside(bool isWhite) const
{
    const int y = isWhite ? 7 : 0;
    return castlingRights[isWhite ? 0 : 1][1] &&
           isPathClearForCastling(y, 0, 4) && !isCheck(isWhite);
}

bool Board::isPathClearForCastling(int row, int startCol, int endCol) const
{
    bool isWhiteTurn = (row == 7);

    if (isCheck(isWhiteTurn)) {
        return false;
    }

    const int step = (startCol < endCol) ? 1 : -1;

    for (int col = startCol + step; col != endCol; col += step) {
        if (board[row][col] != EMPTY) {
            return false;
        }

        if (col != endCol && isSquareUnderAttack(row, col, !isWhiteTurn)) {
            return false;
        }
    }

    return true;
}

bool Board::isStalemate(bool isWhite) const
{
    if (isCheck(isWhite))
        return false;
    return generateAllMoves(isWhite).empty();
}

void Board::generatePawnMoves(int x,
                              int y,
                              bool isWhite,
                              std::vector<Move> &moves) const
{
    const int direction = isWhite ? -1 : 1;
    const int startRow = isWhite ? 6 : 1;

    if (isEmpty(x + direction, y)) {
        moves.emplace_back(x, y, x + direction, y);
        if (x == startRow && isEmpty(x + 2 * direction, y)) {
            moves.emplace_back(x, y, x + 2 * direction, y);
        }
    }

    for (int dy : {-1, 1}) {
        if (isEnemy(x + direction, y + dy, isWhite)) {
            moves.emplace_back(x, y, x + direction, y + dy);
        }
    }
}

void Board::generateRookMoves(int x,
                              int y,
                              bool isWhite,
                              std::vector<Move> &moves) const
{
    const std::pair<int, int> directions[] = {
        {0,  1 },
        {0,  -1},
        {-1, 0 },
        {1,  0 }
    };
    for (const auto &dir : directions) {
        int dx = dir.first;
        int dy = dir.second;

        for (int step = 1; step < 8; step++) {
            int newX = x + step * dx;
            int newY = y + step * dy;

            if (!isInBounds(newX, newY))
                break;

            if (isAlly(newX, newY, isWhite))
                break;

            if (isEnemy(newX, newY, isWhite)) {
                moves.emplace_back(x, y, newX, newY);
                break;
            }

            else {
                moves.emplace_back(x, y, newX, newY);
            }
        }
    }
}

void Board::generateBishopMoves(int x,
                                int y,
                                bool isWhite,
                                std::vector<Move> &moves) const
{
    const std::pair<int, int> directions[] = {
        {1,  1 },
        {-1, -1},
        {-1, 1 },
        {1,  -1}
    };
    for (const auto &dir : directions) {
        int dx = dir.first;
        int dy = dir.second;

        for (int step = 1; step < 8; step++) {
            int newX = x + step * dx;
            int newY = y + step * dy;

            if (!isInBounds(newX, newY))
                break;

            if (isAlly(newX, newY, isWhite))
                break;

            if (isEnemy(newX, newY, isWhite)) {
                moves.emplace_back(x, y, newX, newY);
                break;
            }

            else {
                moves.emplace_back(x, y, newX, newY);
            }
        }
    }
}

void Board::generateKnightMoves(int x,
                                int y,
                                bool isWhite,
                                std::vector<Move> &moves) const
{
    // Все возможные смещения для хода коня (8 вариантов)
    const std::array<std::pair<int, int>, 8> knightMoves = {
        {
         {2, 1},
         {2, -1},
         {-2, 1},
         {-2, -1},
         {1, 2},
         {1, -2},
         {-1, 2},
         {-1, -2},
         }
    };

    for (const auto &[dx, dy] : knightMoves) {
        int newX = x + dx;
        int newY = y + dy;

        if (!isInBounds(newX, newY) || isAlly(newX, newY, isWhite))
            continue;

        if (isEmpty(newX, newY) || isEnemy(newX, newY, isWhite)) {
            moves.emplace_back(x, y, newX, newY);
        }
    }
}

void Board::generateKingMoves(int x,
                              int y,
                              bool isWhite,
                              std::vector<Move> &moves) const
{
    // Обычные ходы короля (1 клетка в любом направлении)
    constexpr std::array<std::pair<int, int>, 8> kingOffsets = {
        {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}
    };

    for (const auto &[dx, dy] : kingOffsets) {
        const int newX = x + dx;
        const int newY = y + dy;

        if (isInBounds(newX, newY) && !isAlly(newX, newY, isWhite)) {
            moves.emplace_back(x, y, newX, newY);
        }
    }

    if (!hasKingMoved(isWhite)) {
        if (canCastleKingside(isWhite)) {
            moves.emplace_back(x, y, x, y + 2);
        }

        if (canCastleQueenside(isWhite)) {
            moves.emplace_back(x, y, x, y - 2);
        }
    }
}

void Board::generateQueenMoves(int x,
                               int y,
                               bool isWhite,
                               std::vector<Move> &moves) const
{
    generateRookMoves(x, y, isWhite, moves);
    generateBishopMoves(x, y, isWhite, moves);
}

bool Board::isSquareUnderAttack(int x, int y, bool byWhite) const
{
    // Проверяем атаку пешками
    int pawnDir = byWhite ? -1 : 1;
    for (int dy : {-1, 1}) {
        if (isInBounds(x + pawnDir, y + dy)) {
            char piece = board[x + pawnDir][y + dy];
            if ((byWhite && piece == 'P') || (!byWhite && piece == 'p')) {
                return true;
            }
        }
    }

    // Проверяем атаку конями
    constexpr std::array<std::pair<int, int>, 8> knightMoves = {
        {{2, 1},
         {2, -1},
         {-2, 1},
         {-2, -1},
         {1, 2},
         {1, -2},
         {-1, 2},
         {-1, -2}}
    };
    for (const auto &[dx, dy] : knightMoves) {
        int nx = x + dx;
        int ny = y + dy;
        if (isInBounds(nx, ny)) {
            char piece = board[nx][ny];
            char neededKnight = byWhite ? 'N' : 'n';
            if (toupper(piece) == 'N' && piece == neededKnight) {
                return true;
            }
        }
    }

    // Проверяем атаку по прямым (ладьи, ферзи)
    constexpr std::array<std::pair<int, int>, 4> rookDirections = {
        {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
    };
    for (const auto &[dx, dy] : rookDirections) {
        for (int step = 1; step < 8; ++step) {
            int nx = x + dx * step;
            int ny = y + dy * step;
            if (!isInBounds(nx, ny)) {
                break;
            }

            char piece = board[nx][ny];
            if (piece != EMPTY) {
                char neededRook = byWhite ? 'R' : 'r';
                char neededQueen = byWhite ? 'Q' : 'q';
                if (piece == neededRook || piece == neededQueen) {
                    return true;
                }
                break;
            }
        }
    }

    // Проверяем атаку по диагоналям (слоны, ферзи)
    constexpr std::array<std::pair<int, int>, 4> bishopDirections = {
        {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}
    };
    for (const auto &[dx, dy] : bishopDirections) {
        for (int step = 1; step < 8; ++step) {
            int nx = x + dx * step;
            int ny = y + dy * step;
            if (!isInBounds(nx, ny))
                break;

            char piece = board[nx][ny];
            if (piece != EMPTY) {
                char neededBishop = byWhite ? 'B' : 'b';
                char neededQueen = byWhite ? 'Q' : 'q';
                if (piece == neededBishop || piece == neededQueen) {
                    return true;
                }
                break;
            }
        }
    }

    // Проверяем атаку королем
    constexpr std::array<std::pair<int, int>, 8> kingMoves = {
        {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}
    };
    for (const auto &[dx, dy] : kingMoves) {
        int nx = x + dx;
        int ny = y + dy;
        if (isInBounds(nx, ny)) {
            char piece = board[nx][ny];
            char neededKing = byWhite ? 'K' : 'k';
            if (piece == neededKing) {
                return true;
            }
        }
    }

    return false;
}

std::vector<Move> Board::generateAllMoves(bool isWhite) const
{
    std::vector<Move> moves;
    moves.reserve(100);

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if ((isWhite && isupper(board[i][j])) ||
                (!isWhite && islower(board[i][j]))) {
                switch (toupper(board[i][j])) {
                case PAWN:
                    generatePawnMoves(i, j, isWhite, moves);
                    break;
                case KNIGHT:
                    generateKnightMoves(i, j, isWhite, moves);
                    break;
                case BISHOP:
                    generateBishopMoves(i, j, isWhite, moves);
                    break;
                case ROOK:
                    generateRookMoves(i, j, isWhite, moves);
                    break;
                case QUEEN:
                    generateQueenMoves(i, j, isWhite, moves);
                    break;
                case KING:
                    generateKingMoves(i, j, isWhite, moves);
                    break;
                }
            }
        }
    }

    std::vector<Move> validMoves;
    for (const Move &move : moves) {
        Board tempBoard = *this;

        char movingPiece = tempBoard.board[move.fromX][move.fromY];
        tempBoard.board[move.toX][move.toY] = movingPiece;
        tempBoard.board[move.fromX][move.fromY] = EMPTY;

        if (!tempBoard.isCheck(isWhite)) {
            validMoves.push_back(move);
        }
    }

    // Проверка ходов
    // for (Move move : validMoves){
    //     std::cout << move << "; ";
    // }
    // std::cout << "\n";

    return validMoves;
}