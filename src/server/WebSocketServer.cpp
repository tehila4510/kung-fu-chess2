#include "server/WebSocketServer.h"

#include "protocol/StateSerializer.h"
#include "server/MatchQueue.h"

#include <chrono>
#include <iostream>

WebSocketServer::WebSocketServer(const std::string& dbPath)
    : usersRepo_(dbPath),
      users_(usersRepo_),
      auth_(users_),
      registry_(),
      matchCoordinator_(registry_),
      roomManager_(registry_, users_, std::cout, "assets/sounds", std::cout) {
    server_.init_asio();
    server_.set_reuse_addr(true);
    server_.clear_access_channels(websocketpp::log::alevel::all);
    server_.set_access_channels(websocketpp::log::alevel::connect |
                                websocketpp::log::alevel::disconnect |
                                websocketpp::log::alevel::app);

    server_.set_open_handler([this](ConnectionHdl hdl) { onOpen(hdl); });
    server_.set_close_handler([this](ConnectionHdl hdl) { onClose(hdl); });
    server_.set_message_handler(
        [this](ConnectionHdl hdl, WsServer::message_ptr msg) { onMessage(hdl, msg); });
}

WebSocketServer::~WebSocketServer() {
    running_ = false;
    roomManager_.clear();
}

void WebSocketServer::sendJson(ConnectionHdl hdl, const std::string& payload) {
    websocketpp::lib::error_code ec;
    server_.send(hdl, payload, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "send failed: " << ec.message() << '\n';
    }
}

void WebSocketServer::sendAll(const std::vector<OutboundMessage>& messages) {
    for (const OutboundMessage& message : messages) {
        sendJson(message.hdl, message.payload);
    }
}

void WebSocketServer::onOpen(ConnectionHdl hdl) {
    // Accept all connections. Players AUTH then PLAY; extra non-play clients
    // use Viewer (stub until Rooms). Do not reject with server_full.
    registry_.registerConnection(hdl);
    sendJson(hdl, protocol::serializeAuthRequiredJson());
    std::cout << "Client connected — AUTH required (" << registry_.size()
              << " connected)\n";
}

void WebSocketServer::onClose(ConnectionHdl hdl) {
    const ClientConn* clientPtr = registry_.getClient(hdl);
    if (clientPtr == nullptr) {
        return;
    }
    const ClientConn client = *clientPtr;

    matchCoordinator_.removeFromQueue(client.playerId);

    if (client.state == ClientState::Seated && client.matchId >= 0) {
        // TODO: implement 20s auto-resign with countdown per spec
        sendAll(roomManager_.tearDownMatch(client.matchId, hdl));
        std::cout << "Player " << client.username << " disconnected from match "
                  << client.matchId << '\n';
    }

    registry_.removeConnection(hdl);
}

void WebSocketServer::handleAuth(ConnectionHdl hdl, const std::string& line) {
    try {
        const AuthCommandResult auth = auth_.handle(line);
        if (!auth.ok) {
            sendJson(hdl, protocol::serializeErrorJson(auth.reason));
            return;
        }

        const ClientConn* client = registry_.getClient(hdl);
        if (client == nullptr) {
            return;
        }
        if (client->state != ClientState::PendingAuth) {
            sendJson(hdl, protocol::serializeErrorJson("already_authenticated"));
            return;
        }
        if (registry_.usernameAlreadyConnected(auth.username)) {
            sendJson(hdl, protocol::serializeErrorJson("already_logged_in"));
            return;
        }

        // When a match already has both seats filled, further AUTHs are viewers
        // (not rejected). Full spectator broadcast is Rooms TODO.
        // TODO: full viewer support (Rooms feature)
        if (registry_.seatedCount() >= 2) {
            registry_.setViewer(hdl, auth.username, auth.rating);
            sendJson(hdl, protocol::serializeAuthOkJson(auth.username, auth.rating,
                                                        "viewer"));
            std::cout << "Viewer " << auth.username << " authenticated (rating "
                      << auth.rating << ")\n";
            return;
        }

        registry_.setAuthenticated(hdl, auth.username, auth.rating);
        sendJson(hdl, protocol::serializeAuthOkJson(auth.username, auth.rating,
                                                    "player"));
        std::cout << "Player " << auth.username << " authenticated (rating "
                  << auth.rating << ")\n";
    } catch (const std::exception& ex) {
        std::cerr << "AUTH failed: " << ex.what() << '\n';
        sendJson(hdl, protocol::serializeErrorJson("runtime_error"));
    }
}

void WebSocketServer::onMessage(ConnectionHdl hdl, WsServer::message_ptr msg) {
    const std::string line = msg->get_payload();
    const ClientConn* client = registry_.getClient(hdl);
    if (client == nullptr) {
        return;
    }

    if (client->state == ClientState::PendingAuth) {
        handleAuth(hdl, line);
        return;
    }

    if (client->state == ClientState::Viewer) {
        // TODO: full viewer support (Rooms feature)
        if (line == "PLAY") {
            sendJson(hdl, protocol::serializeErrorJson("viewer_cannot_play"));
        } else {
            sendJson(hdl, protocol::serializeErrorJson("viewer_readonly"));
        }
        return;
    }

    if (client->state == ClientState::Authenticated ||
        client->state == ClientState::Queued) {
        if (line == "PLAY") {
            const MatchPlayResult play = matchCoordinator_.play(hdl);
            if (play.kind == MatchPlayResult::Kind::Rejected) {
                sendJson(hdl, protocol::serializeErrorJson(play.rejectReason));
                return;
            }
            const ClientConn* after = registry_.getClient(hdl);
            if (after != nullptr) {
                std::cout << "Player " << after->username
                          << " entered queue (rating " << after->rating << ")\n";
            }
            if (play.kind == MatchPlayResult::Kind::Searching) {
                sendJson(hdl, protocol::serializeSearchingJson());
                return;
            }
            sendAll(roomManager_.createMatch(play.white, play.black));
            return;
        }
        sendJson(hdl, protocol::serializeErrorJson("not_seated"));
        return;
    }

    if (client->state != ClientState::Seated) {
        sendJson(hdl, protocol::serializeErrorJson("not_seated"));
        return;
    }

    const RoomCommandResult seated = roomManager_.handleSeatedCommand(hdl, line);
    if (!seated.errorReason.empty()) {
        sendJson(hdl, protocol::serializeErrorJson(seated.errorReason));
        return;
    }
    sendAll(seated.messages);
}

void WebSocketServer::scheduleTick() {
    if (!tickTimer_) {
        return;
    }
    tickTimer_->expires_after(std::chrono::milliseconds(kTickMs));
    tickTimer_->async_wait([this](const asio::error_code& ec) { onTick(ec); });
}

void WebSocketServer::onTick(const asio::error_code& ec) {
    if (ec || !running_) {
        return;
    }

    const auto expired =
        matchCoordinator_.expire(std::chrono::steady_clock::now());
    for (const MatchExpireEvent& event : expired) {
        const ClientConn* client = registry_.getClient(event.hdl);
        if (client != nullptr) {
            std::cout << "Matchmaking timeout for " << client->username << '\n';
        }
        sendJson(event.hdl, protocol::serializeErrorJson("match_not_found"));
    }

    sendAll(roomManager_.tickAll(kTickMs));
    scheduleTick();
}

int WebSocketServer::run() {
    try {
        server_.listen(kPort);
        server_.start_accept();

        tickTimer_ = std::make_unique<asio::steady_timer>(server_.get_io_service());
        running_ = true;
        scheduleTick();

        std::cout << "Kung Fu Chess WebSocket server listening on port " << kPort
                  << '\n';
        std::cout << "Flow: AUTH <user> <pass> then PLAY to matchmake\n";
        std::cout << "Matchmaking: ELO +/-" << MatchQueue::kEloWindow
                  << ", timeout " << (MatchQueue::kQueueTimeoutMs / 1000)
                  << "s\n";
        std::cout << "Commands after match: WMe2e4 | WJe2 | WAIT 100 | STATE\n";

        server_.run();
        running_ = false;
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Server failed: " << ex.what() << '\n';
        running_ = false;
        return 1;
    }
}
