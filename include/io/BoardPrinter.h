#ifndef BOARD_PRINTER_H
#define BOARD_PRINTER_H

#include "model/Board.h"

#include <iosfwd>

class BoardPrinter {
public:
    // Render to an explicit stream.
    void print(const Board& board, std::ostream& out) const;

    // Render to std::cout.
    void print(const Board& board) const;
};

#endif
