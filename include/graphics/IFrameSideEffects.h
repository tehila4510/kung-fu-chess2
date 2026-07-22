#ifndef IFRAME_SIDE_EFFECTS_H
#define IFRAME_SIDE_EFFECTS_H

#include "bus/EventBus.h"
#include "bus/GameEvent.h"
#include "engine/GameEngine.h"
#include "model/GameState.h"
#include "realtime/RealTimeArbiter.h"

namespace graphics {

// Offline-only arrival / score / game-end publishing. Online uses NoOp.
class IFrameSideEffects {
public:
    virtual ~IFrameSideEffects() = default;

    virtual GameSnapshot captureBeforeAdvance() = 0;
    virtual void onAfterAdvance(const GameSnapshot& before) = 0;
    virtual long long elapsedMs() const = 0;
};

class NoOpFrameSideEffects : public IFrameSideEffects {
public:
    GameSnapshot captureBeforeAdvance() override;
    void onAfterAdvance(const GameSnapshot& before) override;
    long long elapsedMs() const override;
};

class LocalFrameSideEffects : public IFrameSideEffects {
public:
    LocalFrameSideEffects(GameEngine& engine, EventBus& bus);

    GameSnapshot captureBeforeAdvance() override;
    void onAfterAdvance(const GameSnapshot& before) override;
    long long elapsedMs() const override;

private:
    void publish(const GameEvent& event);
    void publishArrivals(const GameSnapshot& before, const GameSnapshot& after);

    GameEngine& engine_;
    EventBus& bus_;
    int whiteScore_ = 0;
    int blackScore_ = 0;
};

}  // namespace graphics

#endif
