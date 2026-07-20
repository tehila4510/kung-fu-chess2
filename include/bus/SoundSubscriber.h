#ifndef SOUND_SUBSCRIBER_H
#define SOUND_SUBSCRIBER_H

#include "bus/IGameEventListener.h"

#include <ostream>
#include <string>

// Plays WAV cues from a sounds directory (Windows PlaySound). Also logs cues.
class SoundSubscriber : public IGameEventListener {
    std::string soundsDir_;
    std::ostream& out_;

    void playCue(const char* cue) const;

public:
    SoundSubscriber(std::string soundsDir, std::ostream& out);
    void onEvent(const GameEvent& event) override;
};

#endif
