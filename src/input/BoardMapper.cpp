#include "input/BoardMapper.h"
#include "model/Position.h"

Position BoardMapper::pixelToCell(int x, int y) const {
    return { y / cellSize, x / cellSize };
}