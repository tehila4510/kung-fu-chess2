#include "client/RemoteStateSource.h"

#include "protocol/StateDeserializer.h"

namespace client {

RemoteStateSource::RemoteStateSource(InboundMessageQueue& inbound)
    : inbound_(inbound) {
    clearBoardCache();
    enterLobbyStatus();
}

void RemoteStateSource::setIdentity(const std::string& username, int rating) {
    username_ = username;
    rating_ = rating;
    enterLobbyStatus();
}

void RemoteStateSource::advance(int /*dtMs*/) {
    const std::vector<std::string> messages = inbound_.drain();
    for (const std::string& raw : messages) {
        applyMessage(raw);
    }
}

GameSnapshot RemoteStateSource::getSnapshot() const {
    return snapshot_;
}

std::vector<MotionView> RemoteStateSource::activeMotions() const {
    return motions_;
}

std::vector<RestView> RemoteStateSource::activeRests() const {
    return rests_;
}

bool RemoteStateSource::isGameOver() const {
    return snapshot_.gameOver;
}

MatchPhase RemoteStateSource::phase() const {
    return phase_;
}

char RemoteStateSource::assignedColor() const {
    return assignedColor_;
}

const std::string& RemoteStateSource::username() const {
    return username_;
}

int RemoteStateSource::rating() const {
    return rating_;
}

const std::string& RemoteStateSource::statusLine() const {
    return statusLine_;
}

const std::string& RemoteStateSource::bannerText() const {
    return bannerText_;
}

bool RemoteStateSource::isPlayVisible() const {
    return phase_ == MatchPhase::Lobby || phase_ == MatchPhase::Disconnected;
}

void RemoteStateSource::notePlayRequested() {
    if (phase_ == MatchPhase::Lobby || phase_ == MatchPhase::Disconnected) {
        phase_ = MatchPhase::Searching;
        bannerText_.clear();
        statusLine_ = "Searching for opponent...";
    }
}

void RemoteStateSource::clearBoardCache() {
    snapshot_ = GameSnapshot{};
    snapshot_.gameOver = false;
    motions_.clear();
    rests_.clear();
}

void RemoteStateSource::enterLobbyStatus() {
    phase_ = MatchPhase::Lobby;
    assignedColor_ = '?';
    bannerText_.clear();
    clearBoardCache();
    statusLine_ = "Connected as " + username_ + ", rating " +
                  std::to_string(rating_) + ". Click Play.";
}

void RemoteStateSource::applyMessage(const std::string& raw) {
    const protocol::RemoteMessage message =
        protocol::deserializeRemoteMessage(raw);

    switch (message.kind) {
        case protocol::RemoteMessageKind::Searching:
            phase_ = MatchPhase::Searching;
            bannerText_.clear();
            statusLine_ = "Searching for opponent...";
            break;
        case protocol::RemoteMessageKind::Welcome:
            assignedColor_ = message.welcome.color;
            phase_ = MatchPhase::Seated;
            bannerText_.clear();
            statusLine_ = "Seated as " + std::string(1, assignedColor_) +
                          ". Waiting for board...";
            break;
        case protocol::RemoteMessageKind::State:
            phase_ = MatchPhase::Seated;
            bannerText_.clear();
            snapshot_ = message.state.snapshot;
            motions_ = message.state.motions;
            rests_ = message.state.rests;
            statusLine_ = "Playing as " + std::string(1, assignedColor_);
            if (snapshot_.gameOver) {
                statusLine_ += " — GAME OVER";
            }
            break;
        case protocol::RemoteMessageKind::Error:
            if (message.reason == "match_not_found") {
                phase_ = MatchPhase::Lobby;
                clearBoardCache();
                bannerText_.clear();
                statusLine_ = "No match found. Click Play to try again.";
            } else {
                statusLine_ = "Error: " + message.reason;
            }
            break;
        case protocol::RemoteMessageKind::Disconnected:
            phase_ = MatchPhase::Disconnected;
            clearBoardCache();
            assignedColor_ = '?';
            bannerText_ = "Disconnected from server";
            statusLine_ = "Disconnected. Click Play when reconnected, or restart.";
            break;
        default:
            break;
    }
}

}  // namespace client
