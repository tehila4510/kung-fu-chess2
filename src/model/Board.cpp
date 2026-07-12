#include "model/Board.h"

Board::Board(std::vector<std::vector<std::string>> initialGrid)
    : grid(std::move(initialGrid)),
      rows(static_cast<int>(grid.size())),
      cols(grid.empty() ? 0 : static_cast<int>(grid.front().size())) {}

bool Board::isWithinBounds(const Position& pos) const {
    return pos.row >= 0 && pos.row < rows && pos.col >= 0 && pos.col < cols;
}

std::string Board::getCell(const Position& pos) const {
    return grid[pos.row][pos.col];
}

void Board::setCell(const Position& pos, const std::string& val) {
    grid[pos.row][pos.col] = val;
}

bool Board::isEmpty(const Position& pos) const {
    return grid[pos.row][pos.col] == ".";
}

bool Board::isFriendly(const Position& pos, char color) const {
    if (isEmpty(pos)) return false;
    return grid[pos.row][pos.col][0] == color;
}