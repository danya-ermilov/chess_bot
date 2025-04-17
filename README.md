# Chess Engine

Простой шахматный движок с возможностью игры против ИИ или между двумя игроками.

## Особенности

- Поддержка всех стандартных шахматных правил
- Возможность игры за белых или черных
- Консольный интерфейс
- Базовый ИИ для игры против компьютера
- Отображение доски в консоли

## Требования

- Компилятор C++17 или новее
- CMake (версия 3.10+)

## Сборка и запуск

1. Клонируйте репозиторий:
```bash
git clone https://github.com/danya-ermilov/chess_bot.git
cd chess_bot
```

## Структура проекта
```bash
chess_bot/
├── include/            # Заголовочные файлы
│   ├── Board.h         # Логика шахматной доски
│   └── Engine.h        # Движок
├── src/                # Исходные файлы
│   ├── Board.cpp
│   ├── Engine.cpp
│   └── main.cpp        # Основной игровой цикл
├── Makefile            # Файл сборки
└── README.md           # Этот файл
```
