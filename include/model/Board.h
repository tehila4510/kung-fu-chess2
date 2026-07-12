#ifndef MODEL_BOARD_H
#define MODEL_BOARD_H
#include "model/Position.h"
#include <vector>
#include <string>

class Board {
    std::vector<std::vector<std::string>> grid;
    int rows = 0, cols = 0;
public:
    Board() = default;
    explicit Board(std::vector<std::vector<std::string>> initialGrid);

    bool isWithinBounds(const Position& p) const;
    std::string getCell(const Position& p) const;
    void setCell(const Position& p, const std::string& v);
    bool isEmpty(const Position& p) const;
    bool isFriendly(const Position& p, char color) const;
    int getRowCount() const { return rows; }
    int getColCount() const { return cols; }
};
#endif