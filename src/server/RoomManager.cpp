#include "server/RoomManager.h"

#include "protocol/StateSerializer.h"

#include <iostream>

namespace {

bool sameConnection(ConnectionHdl a, ConnectionHdl b) {
    return !a.owner_before(b) && !b.owner_before(a);
}

}  // namespace

RoomManager::RoomManager(ConnectionRegistry& registry, UserService& users,
                         std::ostream& moveLogOut, std::string soundsDir,
                         std::ostream& soundOut)
    : registry_(registry),
      users_(users),
      moveLog_(moveLogOut),
      sound_(std::move(soundsDir), soundOut) {}

void RoomManager::appendBroadcast(const MatchRoom& room,
                                  const std::string& payload,
                                  std::vector<OutboundMessage>& out) const {
    if (room.hasWhite()) {
        out.push_back(OutboundMessage{room.whiteHdl(), payload});
    }
    if (room.hasBlack()) {
        out.push_back(OutboundMessage{room.blackHdl(), payload});
    }
    // TODO: full viewer support (Rooms feature)
}

std::vector<OutboundMessage> RoomManager::createMatch(ConnectionHdl white,
                                                      ConnectionHdl black) {
    std::vector<OutboundMessage> out;
    ClientConn* whiteClient = registry_.getClient(white);
    ClientConn* blackClient = registry_.getClient(black);
    if (whiteClient == nullptr || blackClient == nullptr) {
        return out;
    }

    const int matchId = nextMatchId_++;
    auto room = std::make_unique<MatchRoom>(matchId, users_, &moveLog_, &sound_);
    room->seatPlayers(white, whiteClient->username, black, blackClient->username);

    registry_.setSeated(white, matchId, 'W');
    registry_.setSeated(black, matchId, 'B');

    const auto events = room->session().drainEvents();
    const std::string welcomeW = protocol::serializeWelcomeJson('W');
    const std::string welcomeB = protocol::serializeWelcomeJson('B');
    const std::string stateW = protocol::serializeGameStateJson(
        room->session().engine(), events, "ok", "welcome");
    const std::string stateB = protocol::serializeGameStateJson(
        room->session().engine(), events, "ok", "welcome");
    out.push_back(OutboundMessage{white, welcomeW});
    out.push_back(OutboundMessage{white, stateW});
    out.push_back(OutboundMessage{black, welcomeB});
    out.push_back(OutboundMessage{black, stateB});

    std::cout << "Match " << matchId << ": " << whiteClient->username
              << " (W) vs " << blackClient->username << " (B)\n";

    rooms_[matchId] = std::move(room);
    return out;
}

std::vector<OutboundMessage> RoomManager::tearDownMatch(
    int matchId, ConnectionHdl exceptHdl) {
    std::vector<OutboundMessage> out;
    auto roomIt = rooms_.find(matchId);
    if (roomIt == rooms_.end()) {
        return out;
    }
    MatchRoom& room = *roomIt->second;

    auto resetOpponent = [this, &exceptHdl, &out](ConnectionHdl other) {
        if (sameConnection(other, exceptHdl)) {
            return;
        }
        if (registry_.getClient(other) == nullptr) {
            return;
        }
        out.push_back(OutboundMessage{
            other, protocol::serializeErrorJson("opponent_disconnected")});
        registry_.clearSeat(other);
    };

    // TODO: implement 20s auto-resign with countdown per spec
    if (room.hasWhite()) {
        resetOpponent(room.whiteHdl());
    }
    if (room.hasBlack()) {
        resetOpponent(room.blackHdl());
    }
    rooms_.erase(roomIt);
    return out;
}

RoomCommandResult RoomManager::handleSeatedCommand(ConnectionHdl hdl,
                                                   const std::string& line) {
    RoomCommandResult result;
    const ClientConn* client = registry_.getClient(hdl);
    if (client == nullptr || client->state != ClientState::Seated ||
        client->matchId < 0) {
        result.errorReason = "not_seated";
        return result;
    }

    auto roomIt = rooms_.find(client->matchId);
    if (roomIt == rooms_.end()) {
        result.errorReason = "match_gone";
        return result;
    }

    MatchRoom& room = *roomIt->second;
    const SessionResult sessionResult =
        room.session().handleCommand(client->color, line);
    const std::string status = sessionResult.accepted ? "ok" : "rejected";
    appendBroadcast(room,
                    protocol::serializeGameStateJson(room.session().engine(),
                                                     sessionResult.events, status,
                                                     sessionResult.reason),
                    result.messages);
    return result;
}

std::vector<OutboundMessage> RoomManager::tickAll(int ms) {
    std::vector<OutboundMessage> out;
    for (auto& entry : rooms_) {
        MatchRoom& room = *entry.second;
        GameSession& session = room.session();
        const bool hadMotion = !session.engine().activeMotions().empty();
        const bool hadRest = !session.engine().activeRests().empty();
        const SessionResult sessionResult = session.tick(ms);
        const bool hasMotion = !session.engine().activeMotions().empty();
        const bool hasRest = !session.engine().activeRests().empty();

        if (!sessionResult.events.empty() || hadMotion || hadRest || hasMotion ||
            hasRest) {
            appendBroadcast(room,
                            protocol::serializeGameStateJson(
                                session.engine(), sessionResult.events, "ok",
                                "tick"),
                            out);
        }
    }
    return out;
}

void RoomManager::clear() {
    rooms_.clear();
}
