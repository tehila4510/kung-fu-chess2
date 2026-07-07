#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>

struct Position {
    int row;
    int col;
    bool isValid() const { return row >= 0 && col >= 0; }
};

class Board {
private:
    std::vector<std::vector<std::string>> grid;
    int rows = 0;
    int cols = 0;
    const int CELL_SIZE = 100;

public:
    bool loadFromLines(const std::vector<std::string>& lines, size_t& index);
    void print() const;

    Position pixelToCell(int x, int y) const;
    bool isWithinBounds(const Position& pos) const;
    std::string getCell(const Position& pos) const;
    void setCell(const Position& pos, const std::string& val);
    bool isFriendly(const Position& pos, char color) const;
    bool isEmpty(const Position& pos) const;
};

#endif