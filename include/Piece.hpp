#pragma once

#include "Types.hpp"
#include <vector>

struct PieceCell {
    int dx{};
    int dy{};
    BlockType type{BlockType::Empty};
};

class Piece {
public:
    PieceShape shape{PieceShape::Single};
    int x{0};
    int y{0};
    std::vector<PieceCell> cells;

    void rotate();
};
