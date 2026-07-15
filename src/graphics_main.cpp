<<<<<<< HEAD
// Graphics entry point: renders the initial board with animated pieces.
// No game logic yet — pieces just play their looping "idle" animation.

#include "graphics/Animation.h"
#include "graphics/AnimationCache.h"
#include "graphics/AssetPaths.h"
#include "graphics/BoardLayout.h"
#include "model/Position.h"
#include "view/Img.h"
#include "view/Renderer.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

const std::string kWindowName = "Kung Fu Chess";
const std::string kIdleState = "idle";
constexpr int kFrameWaitMs = 15;

struct PieceAnimation {
    Position cell;
    graphics::Animation animation;
};

std::vector<PieceAnimation> buildPieceAnimations(const graphics::BoardLayout& layout,
                                                 std::pair<int, int> cell_size,
                                                 graphics::AnimationCache& cache) {
    std::vector<PieceAnimation> pieces;

    for (int row = 0; row < layout.rows; ++row) {
        const std::vector<std::string>& cells = layout.cells[row];
        for (int col = 0; col < static_cast<int>(cells.size()); ++col) {
            const std::string& code = cells[col];
            if (code.empty()) {
                continue;
            }

            const graphics::AnimationSpec& spec =
                cache.get(code, kIdleState, cell_size);
            pieces.push_back(PieceAnimation{
                Position{row, col},
                graphics::Animation(spec.frames, spec.fps, spec.loop)});
        }
    }

    return pieces;
}

// Centre a sprite inside its cell (idle sprites keep aspect, so they are
// usually smaller than the cell).
view::PlacedSprite placeSprite(const view::Img& sprite, const Position& cell,
                         int cell_w, int cell_h) {
    const int x = cell.col * cell_w + (cell_w - sprite.width()) / 2;
    const int y = cell.row * cell_h + (cell_h - sprite.height()) / 2;
    return view::PlacedSprite{&sprite, x, y};
}

bool isExitKey(int key) {
    return key == 27 || key == 'q' || key == 'Q';
}

void updateAnimations(std::vector<PieceAnimation>& pieces, double dt_seconds) {
    for (PieceAnimation& piece : pieces) {
        piece.animation.update(dt_seconds);
    }
}

int renderFrame(view::Renderer& renderer,
                const std::vector<PieceAnimation>& pieces,
                int cell_w, int cell_h) {
    std::vector<view::PlacedSprite> sprites;
    sprites.reserve(pieces.size());
    for (const PieceAnimation& piece : pieces) {
        sprites.push_back(placeSprite(piece.animation.current_frame(),
                                      piece.cell, cell_w, cell_h));
    }
    return renderer.showFrame(sprites, kFrameWaitMs);
}

=======

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
>>>>>>> e6f4cc3a7f7702cf235ad952e4c14f4e815f3039
}  // namespace

int main() {
    try {
<<<<<<< HEAD
        const graphics::BoardLayout layout =
            graphics::BoardLayout::loadFromCsv(graphics::AssetPaths::boardCsv());

        view::Img background;
        background.read(graphics::AssetPaths::boardImage());

        const int cell_w = background.width() / layout.cols;
        const int cell_h = background.height() / layout.rows;
        if (cell_w <= 0 || cell_h <= 0) {
            throw std::runtime_error("Board image is too small for the layout grid.");
        }
        const std::pair<int, int> cell_size{cell_w, cell_h};

        graphics::AnimationCache cache;
        std::vector<PieceAnimation> pieces =
            buildPieceAnimations(layout, cell_size, cache);

        view::Renderer renderer(std::move(background), kWindowName);

        std::cout << "Rendering " << pieces.size() << " animated pieces on a "
                  << layout.rows << "x" << layout.cols << " board.\n";
        std::cout << "Press ESC or Q (or close the window) to exit.\n";

        auto previous_time = std::chrono::steady_clock::now();

        while (true) {
            const auto now_time = std::chrono::steady_clock::now();
            const double dt_seconds =
                std::chrono::duration<double>(now_time - previous_time).count();
            previous_time = now_time;

            updateAnimations(pieces, dt_seconds);
            const int key = renderFrame(renderer, pieces, cell_w, cell_h);
            if (isExitKey(key) || !renderer.isOpen()) {
                break;
            }
        }

        renderer.close();
        std::cout << "Board window closed.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        view::Img::destroyWindows();
=======
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
>>>>>>> e6f4cc3a7f7702cf235ad952e4c14f4e815f3039
        return 1;
    }
}
