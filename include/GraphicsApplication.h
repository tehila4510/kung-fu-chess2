#ifndef GRAPHICS_APPLICATION_H
#define GRAPHICS_APPLICATION_H

#include "bus/EventBus.h"
#include "bus/MoveHistorySubscriber.h"
#include "bus/SoundSubscriber.h"
#include "client/IMatchmaker.h"
#include "graphics/AnimationCache.h"
#include "graphics/BoardLayout.h"
#include "graphics/IBoardInput.h"
#include "graphics/IFrameSideEffects.h"
#include "graphics/IFrameStateSource.h"
#include "graphics/PieceVisual.h"
#include "input/BoardMapper.h"
#include "model/Position.h"
#include "realtime/RestView.h"
#include "view/Img.h"
#include "view/Renderer.h"

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class GraphicsApplication {
public:
    // bus may be null (online). When non-null, sound/history subscribe for offline.
    GraphicsApplication(graphics::BoardLayout layout, view::Img background,
                        graphics::IFrameStateSource& frameSource,
                        client::IMatchmaker& matchmaker,
                        graphics::IBoardInput& boardInput,
                        graphics::IFrameSideEffects& sideEffects,
                        EventBus* bus);
    ~GraphicsApplication();

    GraphicsApplication(const GraphicsApplication&) = delete;
    GraphicsApplication& operator=(const GraphicsApplication&) = delete;

    int run();

private:
    void publish(const GameEvent& event);
    void handleMouseClick(const std::optional<view::MouseClick>& click);
    bool hitPlayButton(int x, int y) const;
    std::vector<view::PlacedSprite> buildSprites(double dt_seconds);
    std::vector<view::CellOverlay> buildLegalMoveOverlays(
        const GameSnapshot& snapshot, const std::vector<MotionView>& motions) const;
    std::vector<view::CellOverlay> buildRestOverlays(
        const std::vector<RestView>& rests) const;
    void pruneStaleVisuals(const GameSnapshot& snapshot,
                           const std::vector<MotionView>& motions);
    graphics::PieceVisual& ensureVisual(const Position& cell, const std::string& token);
    std::string pieceAt(const Position& at) const;

    static constexpr int kFrameWaitMs = 15;
    static constexpr int kMaxFrameStepMs = 100;
    static constexpr int kPanelWidth = 220;
    static constexpr int kPlayButtonW = 120;
    static constexpr int kPlayButtonH = 40;
    static const std::string kWindowName;
    static const std::string kSoundsDir;

    graphics::BoardLayout layout_;
    int cell_w_;
    int cell_h_;
    std::pair<int, int> cell_size_;
    int board_w_;
    int panel_w_;
    int play_btn_x_ = 0;
    int play_btn_y_ = 0;

    graphics::IFrameStateSource& frameSource_;
    client::IMatchmaker& matchmaker_;
    graphics::IBoardInput& boardInput_;
    graphics::IFrameSideEffects& sideEffects_;
    EventBus* bus_ = nullptr;

    BoardMapper mapper_;
    graphics::AnimationCache cache_;
    std::map<Position, graphics::PieceVisual> visuals_;
    std::map<Position, RestKind> rest_anim_keys_;
    view::Renderer renderer_;

    SoundSubscriber sound_;
    MoveHistorySubscriber history_;
};

#endif
