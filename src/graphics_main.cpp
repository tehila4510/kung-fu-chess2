
#include "view/Img.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
const std::string kAssetsRoot = "assets";
constexpr int kBoardPixels = 800;  // rendered board size (square, in pixels)

std::vector<std::vector<std::string>> readBoardCsv(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Cannot open board file: " + path);
    }
    std::vector<std::vector<std::string>> grid;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();  // tolerate Windows CRLF
        }
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        grid.push_back(row);
    }
    return grid;
}

std::string pieceIdleFrame(const std::string& pieceCode, int frame) {
    return kAssetsRoot + "/pieces/" + pieceCode + "/states/idle/sprites/" +
           std::to_string(frame) + ".png";
}
}  // namespace

int main() {
    try {
        const auto grid = readBoardCsv(kAssetsRoot + "/pieces/board.csv");
        const int rows = static_cast<int>(grid.size());
        const int cols = rows > 0 ? static_cast<int>(grid[0].size()) : 0;
        if (rows == 0 || cols == 0) {
            throw std::runtime_error("Board is empty.");
        }

        const int cellW = kBoardPixels / cols;
        const int cellH = kBoardPixels / rows;

        Img board;
        board.read(kAssetsRoot + "/board.png", {cellW * cols, cellH * rows});

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < static_cast<int>(grid[r].size()); ++c) {
                const std::string& code = grid[r][c];
                if (code.empty()) {
                    continue;
                }
                Img piece;
                piece.read(pieceIdleFrame(code, 1), {cellW, cellH}, true);
                piece.draw_on(board, c * cellW, r * cellH);
            }
        }

        board.show();
        std::cout << "Graphics smoke test OK (" << rows << "x" << cols << ")\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
