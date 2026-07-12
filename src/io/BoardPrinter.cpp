#include "io/BoardPrinter.h"

#include <iostream>
#include <ostream>

void BoardPrinter::print(const Board& board, std::ostream& out) const {
    const int rows = board.getRowCount();
    const int cols = board.getColCount();
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (c) out << ' ';
            out << board.getCell(Position{ r, c });
        }
        out << '\n';
    }
}

void BoardPrinter::print(const Board& board) const {
    print(board, std::cout);
}
