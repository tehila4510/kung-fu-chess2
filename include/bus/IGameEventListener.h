#ifndef I_GAME_EVENT_LISTENER_H
#define I_GAME_EVENT_LISTENER_H

#include "bus/GameEvent.h"

class IGameEventListener {
public:
    virtual ~IGameEventListener() = default;
    virtual void onEvent(const GameEvent& event) = 0;
};

#endif
