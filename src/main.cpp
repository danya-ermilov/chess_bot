#include "../include/Board.h"
#include "../include/Engine.h"
#include <iostream>

int main() {
    Board board;
    bool isWhiteTurn = true;
    
    while (true) {
        board.print();
        
        try {
            if (isWhiteTurn) {
                std::cout << "Ваш ход (например, e2 e4): ";
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
                // Ход бота (заглушка)
                std::vector<Move> moves = board.generateAllMoves(false);
                if (!moves.empty()) {
                    board.makeMove(moves[0]); // Просто первый возможный ход
                    std::cout << "Бот сходил: " << moves[0].toChessNotation() << "\n";
                    isWhiteTurn = !isWhiteTurn;
                }
            }
            // Проверка конца игры
            if (board.isCheckmate(!isWhiteTurn)) {
                std::cout << (isWhiteTurn ? "Белые" : "Чёрные") << " победили!\n";
                break;
            }

        } catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
    return 0;
}