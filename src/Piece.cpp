#include "Piece.hpp"

void Piece::rotate() {
    if (shape == PieceShape::Single || cells.size() < 2) {
        return;
    }

    const BlockType first = cells[0].type;
    const BlockType second = cells[1].type;

    if (shape == PieceShape::Vertical2) {
        shape = PieceShape::Horizontal2;
        cells = {{0, 0, first}, {1, 0, second}};
    } else {
        shape = PieceShape::Vertical2;
        cells = {{0, 0, first}, {0, 1, second}};
    }
}
