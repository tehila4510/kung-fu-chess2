#include "bus/EventBus.h"

#include <algorithm>

void EventBus::subscribe(IGameEventListener* listener) {
    if (listener == nullptr) {
        return;
    }
    if (std::find(listeners_.begin(), listeners_.end(), listener) != listeners_.end()) {
        return;
    }
    listeners_.push_back(listener);
}

void EventBus::unsubscribe(IGameEventListener* listener) {
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener),
                     listeners_.end());
}

void EventBus::publish(const GameEvent& event) const {
    for (IGameEventListener* listener : listeners_) {
        if (listener != nullptr) {
            listener->onEvent(event);
        }
    }
}
