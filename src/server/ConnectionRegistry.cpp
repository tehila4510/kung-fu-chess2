#include "server/ConnectionRegistry.h"

MatchQueue::PlayerId ConnectionRegistry::registerConnection(ConnectionHdl hdl) {
    ClientConn client;
    client.playerId = nextPlayerId_++;
    clients_[hdl] = client;
    playerToHdl_[client.playerId] = hdl;
    return client.playerId;
}

void ConnectionRegistry::removeConnection(ConnectionHdl hdl) {
    auto clientIt = clients_.find(hdl);
    if (clientIt == clients_.end()) {
        return;
    }
    playerToHdl_.erase(clientIt->second.playerId);
    clients_.erase(clientIt);
}

ClientConn* ConnectionRegistry::getClient(ConnectionHdl hdl) {
    auto it = clients_.find(hdl);
    if (it == clients_.end()) {
        return nullptr;
    }
    return &it->second;
}

const ClientConn* ConnectionRegistry::getClient(ConnectionHdl hdl) const {
    auto it = clients_.find(hdl);
    if (it == clients_.end()) {
        return nullptr;
    }
    return &it->second;
}

std::optional<ConnectionHdl> ConnectionRegistry::hdlForPlayer(
    MatchQueue::PlayerId id) const {
    auto it = playerToHdl_.find(id);
    if (it == playerToHdl_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool ConnectionRegistry::usernameAlreadyConnected(
    const std::string& username) const {
    for (const auto& entry : clients_) {
        if (entry.second.state == ClientState::PendingAuth) {
            continue;
        }
        if (entry.second.username == username) {
            return true;
        }
    }
    return false;
}

int ConnectionRegistry::seatedCount() const {
    int count = 0;
    for (const auto& entry : clients_) {
        if (entry.second.state == ClientState::Seated) {
            ++count;
        }
    }
    return count;
}

std::size_t ConnectionRegistry::size() const {
    return clients_.size();
}

void ConnectionRegistry::setAuthenticated(ConnectionHdl hdl,
                                          const std::string& username,
                                          int rating) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->username = username;
    client->rating = rating;
    client->state = ClientState::Authenticated;
    client->matchId = -1;
    client->color = '?';
}

void ConnectionRegistry::setViewer(ConnectionHdl hdl, const std::string& username,
                                   int rating) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->username = username;
    client->rating = rating;
    client->state = ClientState::Viewer;
    client->matchId = -1;
    client->color = '?';
}

void ConnectionRegistry::setQueued(ConnectionHdl hdl) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->state = ClientState::Queued;
}

void ConnectionRegistry::setAuthenticatedFromQueue(ConnectionHdl hdl) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->state = ClientState::Authenticated;
    client->matchId = -1;
    client->color = '?';
}

void ConnectionRegistry::setSeated(ConnectionHdl hdl, int matchId, char color) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->state = ClientState::Seated;
    client->matchId = matchId;
    client->color = color;
}

void ConnectionRegistry::clearSeat(ConnectionHdl hdl) {
    ClientConn* client = getClient(hdl);
    if (client == nullptr) {
        return;
    }
    client->state = ClientState::Authenticated;
    client->matchId = -1;
    client->color = '?';
}
