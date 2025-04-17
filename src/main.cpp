#include "../include/Board.h"
#include "../include/Engine.h"
#include <cctype>
#include <iostream>

int main()
{
    Board board;
    ChessEngine engine;

    char choice;
    bool userIsWhite = true;
    std::cout << "Выберите сторону (w - белые, b - чёрные): ";
    std::cin >> choice;

    choice = tolower(choice);
    while (choice != 'b' && choice != 'w') {
        std::cout
            << "Некорректный ввод. Введите B для белых или Ч для чёрных: ";
        std::cin >> choice;
        choice = tolower(choice);
    }

    userIsWhite = (choice == 'w');
    bool isWhiteTurn = true;

    std::cout << "Вы играете за " << (userIsWhite ? "белых" : "чёрных")
              << "\n\n";

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
                if (userIsWhite) {
                    std::cout << "Ваш ход: ";
                    std::string from, to;
                    std::cin >> from >> to;

                    Move move = Move::fromChessNotation(from, to);
                    if (board.isValidMove(move, isWhiteTurn)) {
                        board.makeMove(move);
                        isWhiteTurn = !isWhiteTurn;
                    } else {
                        std::cout << "Недопустимый ход! Попробуйте снова.\n";
                    }
                } else {
                    Move botMove = engine.findBestMove(board, true, 4);
                    if (board.makeMove(botMove)) {
                        std::cout << "Ход бота: " << botMove.toChessNotation()
                                  << "\n";
                        isWhiteTurn = !isWhiteTurn;
                    }
                }
            } else {
                if (!userIsWhite) {
                    std::cout << "Ваш ход: ";
                    std::string from, to;
                    std::cin >> from >> to;

                    Move move = Move::fromChessNotation(from, to);
                    if (board.isValidMove(move, isWhiteTurn)) {
                        board.makeMove(move);
                        isWhiteTurn = !isWhiteTurn;
                    } else {
                        std::cout << "Недопустимый ход! Попробуйте снова.\n";
                    }
                } else {
                    Move botMove = engine.findBestMove(board, false, 4);
                    if (board.makeMove(botMove)) {
                        std::cout << "Ход бота: " << botMove.toChessNotation()
                                  << "\n";
                        isWhiteTurn = !isWhiteTurn;
                    }
                }
            }
        } catch (const std::exception &e) {
            std::cout << "Ошибка: " << e.what() << "\n";
            break;
        }
    }
    return 0;
}