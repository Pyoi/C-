#pragma once

#include "Types.hpp"
#include <array>

class Board {
public:
    Board();

    void clear();
    bool isInside(int row, int col) const;
    bool isEmpty(int row, int col) const;
    BlockType getCell(int row, int col) const;
    void setCell(int row, int col, BlockType type);
    void applyGravity();

private:
    std::array<std::array<BlockType, COLS>, ROWS> grid_{};
};
