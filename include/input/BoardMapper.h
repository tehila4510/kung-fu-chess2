#ifndef BOARD_MAPPER_H
#define BOARD_MAPPER_H
#include "model/Position.h"

class BoardMapper {
private:
    int cellSize=100;
public:
    BoardMapper(int size) : cellSize(size) {}
    Position pixelToCell(int x, int y) const ;
};
#endif
