#ifndef SOUND_SUBSCRIBER_H
#define SOUND_SUBSCRIBER_H

#include "bus/IGameEventListener.h"

#include <ostream>

// Stub sound module — logs trigger names until a real audio backend exists.
class SoundSubscriber : public IGameEventListener {
    std::ostream& out_;
public:
    explicit SoundSubscriber(std::ostream& out);
    void onEvent(const GameEvent& event) override;
};

#endif
