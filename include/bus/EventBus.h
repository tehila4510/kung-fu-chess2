#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "bus/IGameEventListener.h"

#include <vector>

// Observer publish/subscribe bus. Publishers never know concrete subscribers.
// Broadcast structure stays open for a future read-only spectator subscriber
// (added later with Room/Join) — no spectator logic here yet.
class EventBus {
    std::vector<IGameEventListener*> listeners_;
public:
    void subscribe(IGameEventListener* listener);
    void unsubscribe(IGameEventListener* listener);
    void publish(const GameEvent& event) const;
};

#endif
