#include "bus/GameEvent.h"

const char* toString(GameEventType type) {
    switch (type) {
        case GameEventType::MoveMade:         return "MoveMade";
        case GameEventType::JumpMade:         return "JumpMade";
        case GameEventType::PieceCaptured:    return "PieceCaptured";
        case GameEventType::PiecePromoted:    return "PiecePromoted";
        case GameEventType::PieceSelected:    return "PieceSelected";
        case GameEventType::SelectionCleared: return "SelectionCleared";
        case GameEventType::GameEnded:        return "GameEnded";
        case GameEventType::ScoreUpdated:     return "ScoreUpdated";
        case GameEventType::GameStarted:      return "GameStarted";
    }
    return "Unknown";
}
