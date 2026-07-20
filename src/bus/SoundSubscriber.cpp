#include "bus/SoundSubscriber.h"

#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#endif

SoundSubscriber::SoundSubscriber(std::string soundsDir, std::ostream& out)
    : soundsDir_(std::move(soundsDir)), out_(out) {}

void SoundSubscriber::playCue(const char* cue) const {
    if (cue == nullptr || soundsDir_.empty()) {
        return;
    }

#ifdef _WIN32
    const std::string path = soundsDir_ + "\\" + cue + ".wav";
    PlaySoundA(path.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
#else
    (void)cue;
#endif
}

void SoundSubscriber::onEvent(const GameEvent& event) {
    const char* cue = nullptr;
    switch (event.type) {
        case GameEventType::MoveMade:         cue = "move"; break;
        case GameEventType::JumpMade:         cue = "jump"; break;
        case GameEventType::PieceCaptured:    cue = "capture"; break;
        case GameEventType::PiecePromoted:    cue = "promote"; break;
        case GameEventType::PieceSelected:    cue = "select"; break;
        case GameEventType::SelectionCleared: cue = "deselect"; break;
        case GameEventType::GameEnded:        cue = "game_end"; break;
        case GameEventType::GameStarted:      cue = "game_start"; break;
        case GameEventType::ScoreUpdated:     cue = nullptr; break;
    }
    if (cue != nullptr) {
        out_ << "[sound] trigger=" << cue << '\n';
        playCue(cue);
    }
}
