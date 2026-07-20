#ifndef RATING_SUBSCRIBER_H
#define RATING_SUBSCRIBER_H

#include "auth/UserService.h"
#include "bus/IGameEventListener.h"

#include <string>

// Updates both players' ELO ratings when GameEnded fires (winner in event.color).
class RatingSubscriber : public IGameEventListener {
    UserService& users_;
    std::string whiteUsername_;
    std::string blackUsername_;

public:
    explicit RatingSubscriber(UserService& users);
    void setSeat(char color, const std::string& username);
    void clearSeat(char color);
    void onEvent(const GameEvent& event) override;
};

#endif
