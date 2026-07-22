#include "graphics/IFrameSideEffects.h"

#include "protocol/Algebraic.h"

#include <string>

namespace graphics {

GameSnapshot NoOpFrameSideEffects::captureBeforeAdvance() {
    return GameSnapshot{};
}

void NoOpFrameSideEffects::onAfterAdvance(const GameSnapshot&) {}

long long NoOpFrameSideEffects::elapsedMs() const {
    return 0;
}

LocalFrameSideEffects::LocalFrameSideEffects(GameEngine& engine, EventBus& bus)
    : engine_(engine), bus_(bus) {}

GameSnapshot LocalFrameSideEffects::captureBeforeAdvance() {
    return engine_.snapshot();
}

void LocalFrameSideEffects::onAfterAdvance(const GameSnapshot& before) {
    publishArrivals(before, engine_.snapshot());
}

long long LocalFrameSideEffects::elapsedMs() const {
    return engine_.elapsedMs();
}

void LocalFrameSideEffects::publish(const GameEvent& event) {
    GameEvent stamped = event;
    stamped.timeMs = engine_.elapsedMs();
    bus_.publish(stamped);
}

void LocalFrameSideEffects::publishArrivals(const GameSnapshot& before,
                                            const GameSnapshot& after) {
    for (const ArrivalEvent& arrival : engine_.lastArrivals()) {
        if (arrival.piece.size() < 2) {
            continue;
        }
        if (arrival.capturedPiece != "." && !arrival.capturedPiece.empty()) {
            GameEvent captured;
            captured.type = GameEventType::PieceCaptured;
            captured.color = (arrival.piece[0] == 'w') ? 'W' : 'B';
            captured.piece = arrival.piece;
            captured.capturedPiece = arrival.capturedPiece;
            captured.to = protocol::positionToSquare(arrival.at, engine_.rowCount());
            publish(captured);

            const int points = capturePoints(arrival.capturedPiece);
            if (captured.color == 'W') {
                whiteScore_ += points;
            } else {
                blackScore_ += points;
            }
            GameEvent score;
            score.type = GameEventType::ScoreUpdated;
            score.whiteScore = whiteScore_;
            score.blackScore = blackScore_;
            publish(score);
        }
        if (arrival.promoted) {
            GameEvent promoted;
            promoted.type = GameEventType::PiecePromoted;
            promoted.color = (arrival.piece[0] == 'w') ? 'W' : 'B';
            promoted.piece = arrival.piece;
            promoted.reason = "pawn_to_queen";
            promoted.to = protocol::positionToSquare(arrival.at, engine_.rowCount());
            publish(promoted);
        }
    }

    if (!before.gameOver && after.gameOver) {
        GameEvent ended;
        ended.type = GameEventType::GameEnded;
        ended.reason = "king_captured";
        ended.whiteScore = whiteScore_;
        ended.blackScore = blackScore_;
        publish(ended);
    }
}

}  // namespace graphics
