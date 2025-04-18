# Chess Engine ♟️

A lightweight yet powerful chess engine implemented in C++ with AI opponent capabilities.

## Key Features ✨

- Full chess rule implementation:
  - Castling (both kingside and queenside)
  - En passant captures
  - Pawn promotion
  - Check/checkmate/stalemate detection
- Flexible gameplay options:
  - Play as white (`w`)
  - Play as black (`b`)
- Many AI difficulty levels
- Clean console interface with Unicode piece symbols
- Standard algebraic notation support (e.g., e2 e4)

## System Requirements 🛠️

- C++17 compatible compiler (g++/clang/MSVC)
- Linux/macOS/Windows (WSL2 recommended for Windows users)

## Build & Run 🚀

### Building with Makefile
```bash
git clone https://github.com/danya-ermilov/chess_bot.git
cd chess_bot
make
./chess_bot
```

## Project Structure
```bash
chess_bot/
├── Makefile            # Build configuration
├── include/
│   ├── Board.h         # Board logic and move validation
│   ├── Engine.h        # AI search algorithms
└── src/
    ├── Board.cpp       # Rule enforcement
    ├── Engine.cpp      # Minimax with alpha-beta pruning
    └── main.cpp        # Game interface
```
