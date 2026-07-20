#ifndef MOVE_LOG_SUBSCRIBER_H
#define MOVE_LOG_SUBSCRIBER_H

#include "bus/IGameEventListener.h"

#include <ostream>

class MoveLogSubscriber : public IGameEventListener {
    std::ostream& out_;
public:
    explicit MoveLogSubscriber(std::ostream& out);
    void onEvent(const GameEvent& event) override;
};

#endif
