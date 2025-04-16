#include "../include/Board.h"
#include "../include/Engine.h"
#include <iostream>

int main()
{
    Board board;
    ChessEngine engine;
    bool isWhiteTurn = true;

    while (true) {
        board.print();

        if (board.isCheckmate(isWhiteTurn)) {
            std::cout << (!isWhiteTurn ? "Белые" : "Чёрные") << " победили!\n";
            break;
        }

        if (board.isStalemate(isWhiteTurn)) {
            std::cout << "Пат! Ничья.\n";
            break;
        }

        try {
            if (isWhiteTurn) {
                std::cout << "Ваш ход: ";
                std::string from, to;
                std::cin >> from >> to;

                Move move = Move::fromChessNotation(from, to);
                if (board.isValidMove(move, isWhiteTurn)) {
                    board.makeMove(move);
                    isWhiteTurn = !isWhiteTurn;
                } else {
                    std::cout << "Недопустимый ход!\n";
                }
            } else {
                Move botMove = engine.findBestMove(board, false, 5);
                if (board.makeMove(botMove)) {
                    std::cout << "Ход бота: " << botMove.toChessNotation() << "\n";
                    isWhiteTurn = !isWhiteTurn;
                }
            }

        } catch (const std::exception &e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
    return 0;
}