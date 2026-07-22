#ifndef IFRAME_STATE_SOURCE_H
#define IFRAME_STATE_SOURCE_H

#include "model/GameState.h"
#include "realtime/MotionView.h"
#include "realtime/RestView.h"

#include <vector>

namespace graphics {

// Read-only frame state for the graphics loop. Implementations are selected once
// at composition time (local engine vs remote cache).
class IFrameStateSource {
public:
    virtual ~IFrameStateSource() = default;

    // Called once per render frame. Local advances the engine clock; remote drains
    // the inbound WebSocket queue and refreshes cached DTOs.
    virtual void advance(int dtMs) = 0;

    virtual GameSnapshot getSnapshot() const = 0;
    virtual std::vector<MotionView> activeMotions() const = 0;
    virtual std::vector<RestView> activeRests() const = 0;
    virtual bool isGameOver() const = 0;
};

}  // namespace graphics

#endif
