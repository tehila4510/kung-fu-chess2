#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include <string>

enum class GameEventType {
    MoveMade,
    JumpMade,
    PieceCaptured,
    GameEnded,
    ScoreUpdated,
    GameStarted
};

struct GameEvent {
    GameEventType type = GameEventType::MoveMade;
    char color = '?';
    std::string from;
    std::string to;
    std::string piece;
    std::string capturedPiece;
    int whiteScore = 0;
    int blackScore = 0;
    std::string reason;
};

const char* toString(GameEventType type);

#endif
