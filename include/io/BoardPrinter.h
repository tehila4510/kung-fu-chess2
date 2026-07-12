#ifndef BOARD_PRINTER_H
#define BOARD_PRINTER_H

#include "model/Board.h"

#include <iosfwd>

class BoardPrinter {
public:
    void print(const Board& board, std::ostream& out) const;

    void print(const Board& board) const;
};

#endif
