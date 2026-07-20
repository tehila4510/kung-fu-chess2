#ifndef WEB_SOCKET_SERVER_H
#define WEB_SOCKET_SERVER_H

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#ifndef _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_STL_
#endif
// MinGW skips auto C++11 threads in websocketpp; force std::thread (no Boost).
#ifndef _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_THREAD_
#endif

#include "auth/AuthController.h"
#include "auth/UserRepository.h"
#include "auth/UserService.h"
#include "bus/MoveLogSubscriber.h"
#include "bus/RatingSubscriber.h"
#include "bus/SoundSubscriber.h"
#include "server/GameSession.h"

#include <asio.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <map>
#include <memory>
#include <set>
#include <string>

using WsServer = websocketpp::server<websocketpp::config::asio>;
using ConnectionHdl = websocketpp::connection_hdl;

class WebSocketServer {
    static constexpr uint16_t kPort = 9002;
    static constexpr int kTickMs = 50;

    WsServer server_;
    GameSession session_;
    UserRepository usersRepo_;
    UserService users_;
    AuthController auth_;
    MoveLogSubscriber moveLog_;
    SoundSubscriber sound_;
    RatingSubscriber rating_;
    std::set<ConnectionHdl, std::owner_less<ConnectionHdl>> pending_;
    std::map<ConnectionHdl, char, std::owner_less<ConnectionHdl>> seats_;
    std::map<ConnectionHdl, std::string, std::owner_less<ConnectionHdl>> usernames_;
    std::unique_ptr<asio::steady_timer> tickTimer_;
    bool running_ = false;

    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, WsServer::message_ptr msg);
    void handleAuth(ConnectionHdl hdl, const std::string& line);
    void seatAfterAuth(ConnectionHdl hdl, const std::string& username, int rating);
    void scheduleTick();
    void onTick(const asio::error_code& ec);
    void broadcastJson(const std::string& payload);
    void sendJson(ConnectionHdl hdl, const std::string& payload);
    char seatFor(ConnectionHdl hdl) const;
    int seatedCount() const;
    int connectedCount() const;
    bool isPending(ConnectionHdl hdl) const;

public:
    explicit WebSocketServer(const std::string& dbPath = "data/users.db");
    ~WebSocketServer();
    int run();
};

#endif
