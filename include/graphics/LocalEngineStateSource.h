#ifndef LOCAL_ENGINE_STATE_SOURCE_H
#define LOCAL_ENGINE_STATE_SOURCE_H

#include "engine/GameEngine.h"
#include "graphics/BoardLayout.h"
#include "graphics/IFrameStateSource.h"
#include "model/Board.h"

namespace graphics {

// Converts asset CSV tokens (KB/RW) to engine tokens (bK/wR) and builds a Board.
Board boardFromAssetLayout(const BoardLayout& layout);

// Thin wrapper around an existing GameEngine (offline path).
class LocalEngineStateSource : public IFrameStateSource {
public:
    explicit LocalEngineStateSource(GameEngine& engine);

    void advance(int dtMs) override;
    GameSnapshot getSnapshot() const override;
    std::vector<MotionView> activeMotions() const override;
    std::vector<RestView> activeRests() const override;
    bool isGameOver() const override;

    GameEngine& engine();
    const GameEngine& engine() const;

private:
    GameEngine& engine_;
};

}  // namespace graphics

#endif
