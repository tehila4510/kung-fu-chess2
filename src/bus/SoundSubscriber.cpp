#include "bus/SoundSubscriber.h"

SoundSubscriber::SoundSubscriber(std::ostream& out) : out_(out) {}

void SoundSubscriber::onEvent(const GameEvent& event) {
    const char* cue = nullptr;
    switch (event.type) {
        case GameEventType::MoveMade:      cue = "move"; break;
        case GameEventType::JumpMade:      cue = "jump"; break;
        case GameEventType::PieceCaptured: cue = "capture"; break;
        case GameEventType::GameEnded:     cue = "game_end"; break;
        case GameEventType::GameStarted:   cue = "game_start"; break;
        case GameEventType::ScoreUpdated:  cue = nullptr; break;
    }
    if (cue != nullptr) {
        out_ << "[sound] trigger=" << cue << '\n';
    }
}