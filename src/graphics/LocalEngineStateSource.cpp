#include "graphics/LocalEngineStateSource.h"

#include <cctype>
#include <string>
#include <vector>

namespace graphics {
namespace {

std::string assetTokenToEngine(const std::string& asset_token) {
    if (asset_token.empty() || asset_token == ".") {
        return ".";
    }
    if (asset_token.size() != 2) {
        return asset_token;
    }

    const char kind = asset_token[0];
    const char color_letter = asset_token[1];
    if (!std::isalpha(static_cast<unsigned char>(kind)) ||
        !std::isalpha(static_cast<unsigned char>(color_letter))) {
        return asset_token;
    }

    const char color = (color_letter == 'W' || color_letter == 'w') ? 'w' : 'b';
    return std::string(1, color) + std::string(1, kind);
}

}  // namespace

Board boardFromAssetLayout(const BoardLayout& layout) {
    std::vector<std::vector<std::string>> engine_grid;
    engine_grid.reserve(layout.cells.size());
    for (const std::vector<std::string>& row : layout.cells) {
        std::vector<std::string> engine_row;
        engine_row.reserve(row.size());
        for (const std::string& cell : row) {
            engine_row.push_back(assetTokenToEngine(cell));
        }
        engine_grid.push_back(std::move(engine_row));
    }
    return Board(std::move(engine_grid));
}

LocalEngineStateSource::LocalEngineStateSource(GameEngine& engine)
    : engine_(engine) {}

void LocalEngineStateSource::advance(int dtMs) {
    if (dtMs > 0) {
        engine_.wait(dtMs);
    }
}

GameSnapshot LocalEngineStateSource::getSnapshot() const {
    return engine_.snapshot();
}

std::vector<MotionView> LocalEngineStateSource::activeMotions() const {
    return engine_.activeMotions();
}

std::vector<RestView> LocalEngineStateSource::activeRests() const {
    return engine_.activeRests();
}

bool LocalEngineStateSource::isGameOver() const {
    return engine_.isGameOver();
}

GameEngine& LocalEngineStateSource::engine() {
    return engine_;
}

const GameEngine& LocalEngineStateSource::engine() const {
    return engine_;
}

}  // namespace graphics
