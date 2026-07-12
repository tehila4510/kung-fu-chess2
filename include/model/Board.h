#ifndef MODEL_BOARD_H
#define MODEL_BOARD_H
#include "model/Position.h"
#include <vector>
#include <string>

class Board {
    std::vector<std::vector<std::string>> grid;
    int rows = 0, cols = 0;
public:
    bool loadFromLines(const std::vector<std::string>& lines, size_t& index);
    void print() const;
    bool isWithinBounds(const Position& p) const;
    std::string getCell(const Position& p) const;
    void setCell(const Position& p, const std::string& v);
    bool isEmpty(const Position& p) const;
    bool isFriendly(const Position& p, char color) const;
    int getRowCount() const { return rows; }
    int getColCount() const { return cols; }
};
#endif