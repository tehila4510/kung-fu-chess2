#include "server/MatchCoordinator.h"

MatchCoordinator::MatchCoordinator(ConnectionRegistry& registry)
    : registry_(registry) {}

void MatchCoordinator::removeFromQueue(MatchQueue::PlayerId id) {
    queue_.remove(id);
}

MatchPlayResult MatchCoordinator::play(ConnectionHdl hdl) {
    MatchPlayResult result;
    const ClientConn* client = registry_.getClient(hdl);
    if (client == nullptr) {
        result.kind = MatchPlayResult::Kind::Rejected;
        result.rejectReason = "not_authenticated";
        return result;
    }
    if (client->state == ClientState::Viewer) {
        result.kind = MatchPlayResult::Kind::Rejected;
        result.rejectReason = "viewer_cannot_play";
        return result;
    }
    if (client->state == ClientState::Queued) {
        result.kind = MatchPlayResult::Kind::Rejected;
        result.rejectReason = "already_searching";
        return result;
    }
    if (client->state != ClientState::Authenticated) {
        result.kind = MatchPlayResult::Kind::Rejected;
        result.rejectReason = "not_authenticated";
        return result;
    }

    const MatchQueue::PlayerId playerId = client->playerId;
    const int rating = client->rating;
    registry_.setQueued(hdl);
    queue_.enqueue(playerId, rating, std::chrono::steady_clock::now());

    const auto matched = queue_.tryMatch(playerId);
    if (!matched) {
        result.kind = MatchPlayResult::Kind::Searching;
        return result;
    }

    const auto whiteHdl = registry_.hdlForPlayer(matched->white);
    const auto blackHdl = registry_.hdlForPlayer(matched->black);
    if (!whiteHdl || !blackHdl) {
        result.kind = MatchPlayResult::Kind::Searching;
        return result;
    }

    result.kind = MatchPlayResult::Kind::Matched;
    result.white = *whiteHdl;
    result.black = *blackHdl;
    return result;
}

std::vector<MatchExpireEvent> MatchCoordinator::expire(
    MatchQueue::TimePoint now) {
    std::vector<MatchExpireEvent> events;
    const std::vector<MatchQueue::PlayerId> expired = queue_.expire(now);
    for (MatchQueue::PlayerId playerId : expired) {
        const auto hdl = registry_.hdlForPlayer(playerId);
        if (!hdl) {
            continue;
        }
        registry_.setAuthenticatedFromQueue(*hdl);
        events.push_back(MatchExpireEvent{*hdl});
    }
    return events;
}
