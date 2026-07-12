#ifndef BOARD_MAPPER_H
#define BOARD_MAPPER_H
#include "model/Position.h"
#include <optional>

class BoardMapper {
    int cellSize = 100;
    int widthCells = 8;
    int heightCells = 8;
public:
    BoardMapper(int cellSize = 100, int widthCells = 8, int heightCells = 8);
    std::optional<Position> pixelToCell(int x, int y) const;
    int getCellSize() const;
    int getWidthCells() const;
    int getHeightCells() const;
};
#endif
