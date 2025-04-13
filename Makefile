CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
EXEC = chessbot

# Основная сборка
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

lint:
	clang-tidy $(SRCS) --extra-arg="$(CXXFLAGS)"

check-format:
	clang-format --dry-run --Werror $(SRCS)

check-cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem .

# Автоформатирование
format:
	clang-format -i $(SRCS)

# Полная проверка (линтер + стиль + статический анализ)
full-check: lint check-format check-cppcheck

# Очистка
clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean lint format check-format check-cppcheck full-check