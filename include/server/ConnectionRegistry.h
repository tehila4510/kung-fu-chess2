#ifndef CONNECTION_REGISTRY_H
#define CONNECTION_REGISTRY_H

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#ifndef _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_STL_
#endif
#ifndef _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_THREAD_
#endif

#include "server/MatchQueue.h"

#include <websocketpp/common/connection_hdl.hpp>

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

using ConnectionHdl = websocketpp::connection_hdl;

enum class ClientState {
    PendingAuth,
    Authenticated,
    Queued,
    Seated,
    Viewer
};

struct ClientConn {
    ClientState state = ClientState::PendingAuth;
    std::string username;
    int rating = 0;
    MatchQueue::PlayerId playerId = -1;
    int matchId = -1;
    char color = '?';
};

// Owns connection → client records and PlayerId ↔ ConnectionHdl maps.
// No send, no auth credentials, no matchmaking queue.
class ConnectionRegistry {
    std::map<ConnectionHdl, ClientConn, std::owner_less<ConnectionHdl>> clients_;
    std::unordered_map<MatchQueue::PlayerId, ConnectionHdl> playerToHdl_;
    MatchQueue::PlayerId nextPlayerId_ = 1;

public:
    MatchQueue::PlayerId registerConnection(ConnectionHdl hdl);
    void removeConnection(ConnectionHdl hdl);

    ClientConn* getClient(ConnectionHdl hdl);
    const ClientConn* getClient(ConnectionHdl hdl) const;
    std::optional<ConnectionHdl> hdlForPlayer(MatchQueue::PlayerId id) const;
    bool usernameAlreadyConnected(const std::string& username) const;
    int seatedCount() const;
    std::size_t size() const;

    void setAuthenticated(ConnectionHdl hdl, const std::string& username, int rating);
    void setViewer(ConnectionHdl hdl, const std::string& username, int rating);
    void setQueued(ConnectionHdl hdl);
    void setAuthenticatedFromQueue(ConnectionHdl hdl);
    void setSeated(ConnectionHdl hdl, int matchId, char color);
    void clearSeat(ConnectionHdl hdl);
};

#endif
