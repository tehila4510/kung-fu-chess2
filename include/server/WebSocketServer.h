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
#include "server/ConnectionRegistry.h"
#include "server/MatchCoordinator.h"
#include "server/RoomManager.h"

#include <asio.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <memory>
#include <string>
#include <vector>

using WsServer = websocketpp::server<websocketpp::config::asio>;

class WebSocketServer {
    static constexpr uint16_t kPort = 9002;
    static constexpr int kTickMs = 50;

    WsServer server_;
    UserRepository usersRepo_;
    UserService users_;
    AuthController auth_;
    ConnectionRegistry registry_;
    MatchCoordinator matchCoordinator_;
    RoomManager roomManager_;

    std::unique_ptr<asio::steady_timer> tickTimer_;
    bool running_ = false;

    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, WsServer::message_ptr msg);
    void handleAuth(ConnectionHdl hdl, const std::string& line);
    void scheduleTick();
    void onTick(const asio::error_code& ec);
    void sendJson(ConnectionHdl hdl, const std::string& payload);
    void sendAll(const std::vector<OutboundMessage>& messages);

public:
    explicit WebSocketServer(const std::string& dbPath = "data/users.db");
    ~WebSocketServer();
    int run();
};

#endif
