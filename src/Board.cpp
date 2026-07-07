#include "Board.h"
#include <iostream>
#include <sstream>

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static bool isValidToken(const std::string& tok) {
    if (tok == ".") return true;
    if (tok.size() != 2) return false;
    if (tok[0] != 'w' && tok[0] != 'b') return false;
    static const std::string pieces = "KQRBNP";
    return pieces.find(tok[1]) != std::string::npos;
}

bool Board::loadFromLines(const std::vector<std::string>& lines, size_t& i) {
    while (i < lines.size() && trim(lines[i]) != "Board:") i++;
    if (i < lines.size()) i++;

    while (i < lines.size()) {
        std::string t = trim(lines[i]);
        if (t == "Commands:") { i++; break; }
        if (t.empty()) { i++; continue; }

        std::istringstream iss(t);
        std::vector<std::string> tokens;
        std::string tok;
        while (iss >> tok) tokens.push_back(tok);

        for (const auto& tk : tokens) {
            if (!isValidToken(tk)) {
                std::cout << "ERROR UNKNOWN_TOKEN\n";
                return false;
            }
        }
        grid.push_back(tokens);
        i++;
    }

    if (!grid.empty()) {
        rows = grid.size();
        cols = grid[0].size();
        for (const auto& r : grid) {
            if (r.size() != static_cast<size_t>(cols)) {
                std::cout << "ERROR ROW_WIDTH_MISMATCH\n";
                return false;
            }
        }
    }
    return true;
}

Position Board::pixelToCell(int x, int y) const {
    return { y / CELL_SIZE, x / CELL_SIZE };
}

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

void Board::print() const {
    for (const auto& r : grid) {
        for (size_t j = 0; j < r.size(); j++) {
            if (j) std::cout << ' ';
            std::cout << r[j];
        }
        std::cout << '\n';
    }
}