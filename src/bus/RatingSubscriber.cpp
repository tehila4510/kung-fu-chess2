#include "bus/RatingSubscriber.h"

#include <iostream>

RatingSubscriber::RatingSubscriber(UserService& users) : users_(users) {}

void RatingSubscriber::setSeat(char color, const std::string& username) {
    if (color == 'W') {
        whiteUsername_ = username;
    } else if (color == 'B') {
        blackUsername_ = username;
    }
}

void RatingSubscriber::clearSeat(char color) {
    if (color == 'W') {
        whiteUsername_.clear();
    } else if (color == 'B') {
        blackUsername_.clear();
    }
}

void RatingSubscriber::onEvent(const GameEvent& event) {
    if (event.type != GameEventType::GameEnded) {
        return;
    }
    if (whiteUsername_.empty() || blackUsername_.empty()) {
        return;
    }
    if (event.color != 'W' && event.color != 'B') {
        std::cerr << "RatingSubscriber: GameEnded missing winner color\n";
        return;
    }

    try {
        users_.applyGameResult(whiteUsername_, blackUsername_, event.color);
    } catch (const std::exception& ex) {
        std::cerr << "RatingSubscriber failed: " << ex.what() << '\n';
    }
}
