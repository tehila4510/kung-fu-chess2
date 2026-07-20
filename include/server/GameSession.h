#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include "bus/EventBus.h"
#include "bus/GameEvent.h"
#include "engine/GameEngine.h"
#include "protocol/CommandParser.h"

#include <string>
#include <vector>

struct SessionResult {
    bool accepted = false;
    std::string reason;
    std::vector<GameEvent> events;
};

// Thin facade over GameEngine: invokes existing logic and publishes Bus events.
class GameSession {
    GameEngine engine_;
    EventBus bus_;
    int whiteScore_ = 0;
    int blackScore_ = 0;

    class EventCollector;
    EventCollector* collector_ = nullptr;

    void publish(const GameEvent& event);
    void publishArrivals(const GameSnapshot& before, const GameSnapshot& after);
    char engineColor(char wireColor) const;
    std::string pieceAt(const Position& at) const;

public:
    GameSession();
    ~GameSession();

    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;

    EventBus& bus();
    const GameEngine& engine() const;
    GameEngine& engine();

    void startNewGame();
    SessionResult handleCommand(char playerColor, const std::string& line);
    SessionResult tick(int ms);
    std::vector<GameEvent> drainEvents();
};

#endif
