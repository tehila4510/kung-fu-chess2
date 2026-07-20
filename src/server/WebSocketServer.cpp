#include "server/WebSocketServer.h"

#include "protocol/StateSerializer.h"

#include <chrono>
#include <iostream>

WebSocketServer::WebSocketServer(const std::string& dbPath)
    : usersRepo_(dbPath),
      users_(usersRepo_),
      auth_(users_),
      moveLog_(std::cout),
      sound_("assets/sounds", std::cout),
      rating_(users_) {
    session_.bus().subscribe(&moveLog_);
    session_.bus().subscribe(&sound_);
    session_.bus().subscribe(&rating_);

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
    session_.bus().unsubscribe(&moveLog_);
    session_.bus().unsubscribe(&sound_);
    session_.bus().unsubscribe(&rating_);
}

int WebSocketServer::seatedCount() const {
    return static_cast<int>(seats_.size());
}

int WebSocketServer::connectedCount() const {
    return static_cast<int>(pending_.size() + seats_.size());
}

bool WebSocketServer::isPending(ConnectionHdl hdl) const {
    return pending_.find(hdl) != pending_.end();
}

char WebSocketServer::seatFor(ConnectionHdl hdl) const {
    const auto it = seats_.find(hdl);
    if (it == seats_.end()) {
        return '?';
    }
    return it->second;
}

void WebSocketServer::sendJson(ConnectionHdl hdl, const std::string& payload) {
    websocketpp::lib::error_code ec;
    server_.send(hdl, payload, websocketpp::frame::opcode::text, ec);
    if (ec) {
        std::cerr << "send failed: " << ec.message() << '\n';
    }
}

void WebSocketServer::broadcastJson(const std::string& payload) {
    // Fan-out to seated players only. Structure stays open for a future
    // read-only spectator subscriber (Room/Join) — no spectator logic yet.
    for (const auto& entry : seats_) {
        sendJson(entry.first, payload);
    }
}

void WebSocketServer::onOpen(ConnectionHdl hdl) {
    // TODO: spectator support will be added later alongside Room/Join.
    // For now reject any 3rd+ client (no read, no write).
    if (connectedCount() >= 2) {
        sendJson(hdl, protocol::serializeErrorJson("server_full"));
        websocketpp::lib::error_code ec;
        server_.close(hdl, websocketpp::close::status::try_again_later, "server_full",
                      ec);
        return;
    }

    pending_.insert(hdl);
    sendJson(hdl, protocol::serializeAuthRequiredJson());
    std::cout << "Client connected — AUTH required (" << connectedCount() << "/2)\n";
}

void WebSocketServer::onClose(ConnectionHdl hdl) {
    const char color = seatFor(hdl);
    pending_.erase(hdl);
    seats_.erase(hdl);
    usernames_.erase(hdl);
    if (color != '?') {
        rating_.clearSeat(color);
        std::cout << "Player " << color << " disconnected (" << seatedCount()
                  << "/2 seated)\n";
    }
}

void WebSocketServer::seatAfterAuth(ConnectionHdl hdl, const std::string& username,
                                    int rating) {
    pending_.erase(hdl);
    const char color = (seatedCount() == 0) ? 'W' : 'B';
    seats_[hdl] = color;
    usernames_[hdl] = username;
    rating_.setSeat(color, username);

    sendJson(hdl, protocol::serializeAuthOkJson(username, rating, color));
    sendJson(hdl, protocol::serializeWelcomeJson(color));

    const auto events = session_.drainEvents();
    sendJson(hdl, protocol::serializeGameStateJson(session_.engine(), events, "ok",
                                                   "welcome"));

    std::cout << "Player " << username << " seated as " << color << " (rating "
              << rating << ", " << seatedCount() << "/2)\n";
    if (seatedCount() == 2) {
        std::cout << "Both players connected — game live on port " << kPort << '\n';
    }
}

void WebSocketServer::handleAuth(ConnectionHdl hdl, const std::string& line) {
    try {
        const AuthCommandResult auth = auth_.handle(line);
        if (!auth.ok) {
            sendJson(hdl, protocol::serializeErrorJson(auth.reason));
            return;
        }
        if (seatedCount() >= 2) {
            sendJson(hdl, protocol::serializeErrorJson("server_full"));
            return;
        }
        seatAfterAuth(hdl, auth.username, auth.rating);
    } catch (const std::exception& ex) {
        std::cerr << "AUTH failed: " << ex.what() << '\n';
        sendJson(hdl, protocol::serializeErrorJson("runtime_error"));
    }
}

void WebSocketServer::onMessage(ConnectionHdl hdl, WsServer::message_ptr msg) {
    const std::string line = msg->get_payload();

    if (isPending(hdl)) {
        handleAuth(hdl, line);
        return;
    }

    const char color = seatFor(hdl);
    if (color == '?') {
        sendJson(hdl, protocol::serializeErrorJson("not_seated"));
        return;
    }

    const SessionResult result = session_.handleCommand(color, line);
    const std::string status = result.accepted ? "ok" : "rejected";
    broadcastJson(protocol::serializeGameStateJson(session_.engine(), result.events,
                                                   status, result.reason));
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

    const bool hadMotion = !session_.engine().activeMotions().empty();
    const bool hadRest = !session_.engine().activeRests().empty();
    const SessionResult result = session_.tick(kTickMs);
    const bool hasMotion = !session_.engine().activeMotions().empty();
    const bool hasRest = !session_.engine().activeRests().empty();

    if (!result.events.empty() || hadMotion || hadRest || hasMotion || hasRest) {
        broadcastJson(protocol::serializeGameStateJson(session_.engine(), result.events,
                                                       "ok", "tick"));
    }

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
        std::cout << "Waiting for AUTH then White/Black seats...\n";
        std::cout << "Auth: AUTH <username> <password>\n";
        std::cout << "Commands: WMe2e4 | WJe2 | WAIT 100 | STATE\n";

        server_.run();
        running_ = false;
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Server failed: " << ex.what() << '\n';
        running_ = false;
        return 1;
    }
}
