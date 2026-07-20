#include "bus/MoveLogSubscriber.h"

MoveLogSubscriber::MoveLogSubscriber(std::ostream& out) : out_(out) {}

void MoveLogSubscriber::onEvent(const GameEvent& event) {
    out_ << "[move-log] " << toString(event.type);
    if (event.color != '?') {
        out_ << " color=" << event.color;
    }
    if (!event.from.empty()) {
        out_ << " from=" << event.from;
    }
    if (!event.to.empty()) {
        out_ << " to=" << event.to;
    }
    if (!event.piece.empty()) {
        out_ << " piece=" << event.piece;
    }
    if (!event.capturedPiece.empty()) {
        out_ << " captured=" << event.capturedPiece;
    }
    if (!event.reason.empty()) {
        out_ << " reason=" << event.reason;
    }
    out_ << '\n';
}
