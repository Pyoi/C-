#include "Board.hpp"

Board::Board() {
    clear();
}

void Board::clear() {
    for (auto& row : grid_) {
        row.fill(BlockType::Empty);
    }
}

bool Board::isInside(int row, int col) const {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

bool Board::isEmpty(int row, int col) const {
    return isInside(row, col) && grid_[row][col] == BlockType::Empty;
}

BlockType Board::getCell(int row, int col) const {
    return grid_[row][col];
}

void Board::setCell(int row, int col, BlockType type) {
    if (isInside(row, col)) {
        grid_[row][col] = type;
    }
}

void Board::applyGravity() {
    for (int col = 0; col < COLS; ++col) {
        int writeRow = ROWS - 1;
        for (int row = ROWS - 1; row >= 0; --row) {
            if (grid_[row][col] != BlockType::Empty) {
                grid_[writeRow][col] = grid_[row][col];
                if (writeRow != row) {
                    grid_[row][col] = BlockType::Empty;
                }
                --writeRow;
            }
        }
        while (writeRow >= 0) {
            grid_[writeRow][col] = BlockType::Empty;
            --writeRow;
        }
    }
}
