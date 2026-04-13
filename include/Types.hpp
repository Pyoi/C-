#pragma once

#include <cstdint>

constexpr int COLS = 6;
constexpr int ROWS = 12;
constexpr int CELL_SIZE = 48;
constexpr int BOARD_X = 40;
constexpr int BOARD_Y = 40;
constexpr int WINDOW_WIDTH = 520;
constexpr int WINDOW_HEIGHT = 700;

enum class SceneState {
    Title,
    Playing,
    Exiting
};

enum class Difficulty {
    Easy,
    Normal,
    Hard
};

enum class BlockType : std::uint8_t {
    Empty = 0,
    Red,
    Blue,
    Green,
    Yellow,
    Purple,
    Orange,
    Cyan,
    Rainbow
};

enum class PieceShape : std::uint8_t {
    Single,
    Vertical2,
    Horizontal2
};
