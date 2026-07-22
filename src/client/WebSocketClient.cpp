#include "client/WebSocketClient.h"

#include <asio.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include <chrono>
#include <iostream>
#include <mutex>
#include <stdexcept>

namespace client {
namespace {

using WsClient = websocketpp::client<websocketpp::config::asio_client>;
using ConnectionHdl = websocketpp::connection_hdl;

}  // namespace

struct WebSocketClient::Impl {
    WsClient client;
    ConnectionHdl hdl;
    std::mutex hdlMutex;
    bool hdlSet = false;
};

WebSocketClient::WebSocketClient() : impl_(std::make_unique<Impl>()) {}

WebSocketClient::~WebSocketClient() {
    close();
}

void WebSocketClient::connect(const std::string& host, std::uint16_t port) {
    if (ioThread_.joinable()) {
        throw std::runtime_error("WebSocketClient already connected.");
    }

    connected_ = false;
    disconnected_ = false;

    impl_->client.clear_access_channels(websocketpp::log::alevel::all);
    impl_->client.clear_error_channels(websocketpp::log::elevel::all);
    impl_->client.init_asio();
    impl_->client.start_perpetual();

    impl_->client.set_open_handler([this](ConnectionHdl hdl) {
        {
            std::lock_guard<std::mutex> lock(impl_->hdlMutex);
            impl_->hdl = hdl;
            impl_->hdlSet = true;
        }
        connected_ = true;
        disconnected_ = false;
    });

    impl_->client.set_fail_handler([this](ConnectionHdl) {
        connected_ = false;
        disconnected_ = true;
        inbound_.push(R"({"type":"disconnected","reason":"connect_failed"})");
    });

    impl_->client.set_close_handler([this](ConnectionHdl) {
        connected_ = false;
        disconnected_ = true;
        inbound_.push(R"({"type":"disconnected","reason":"connection_closed"})");
    });

    impl_->client.set_message_handler(
        [this](ConnectionHdl, WsClient::message_ptr msg) {
            if (msg) {
                inbound_.push(msg->get_payload());
            }
        });

    const std::string uri = "ws://" + host + ":" + std::to_string(port);
    websocketpp::lib::error_code ec;
    WsClient::connection_ptr con = impl_->client.get_connection(uri, ec);
    if (ec) {
        throw std::runtime_error("WebSocket get_connection failed: " + ec.message());
    }

    impl_->client.connect(con);

    ioThread_ = std::thread([this] {
        try {
            impl_->client.run();
        } catch (const std::exception& ex) {
            std::cerr << "WebSocket I/O thread error: " << ex.what() << '\n';
            connected_ = false;
            disconnected_ = true;
            inbound_.push(R"({"type":"disconnected","reason":"io_error"})");
        }
    });

    // Block until open, fail, or timeout (bootstrap only).
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (!connected_ && !disconnected_) {
        if (std::chrono::steady_clock::now() > deadline) {
            close();
            throw std::runtime_error("WebSocket connect timed out: " + uri);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (!connected_) {
        close();
        throw std::runtime_error("WebSocket connect failed: " + uri);
    }
}

void WebSocketClient::send(const std::string& line) {
    if (!connected_) {
        return;
    }

    ConnectionHdl hdl;
    {
        std::lock_guard<std::mutex> lock(impl_->hdlMutex);
        if (!impl_->hdlSet) {
            return;
        }
        hdl = impl_->hdl;
    }

    // Post onto the client's io_service so send runs on the I/O thread.
    asio::post(impl_->client.get_io_service(), [this, hdl, line] {
        websocketpp::lib::error_code ec;
        impl_->client.send(hdl, line, websocketpp::frame::opcode::text, ec);
        if (ec) {
            std::cerr << "WebSocket send failed: " << ec.message() << '\n';
        }
    });
}

InboundMessageQueue& WebSocketClient::inboundQueue() {
    return inbound_;
}

const InboundMessageQueue& WebSocketClient::inboundQueue() const {
    return inbound_;
}

bool WebSocketClient::isConnected() const {
    return connected_;
}

bool WebSocketClient::wasDisconnected() const {
    return disconnected_;
}

void WebSocketClient::close() {
    connected_ = false;
    try {
        impl_->client.stop_perpetual();
        {
            std::lock_guard<std::mutex> lock(impl_->hdlMutex);
            if (impl_->hdlSet) {
                websocketpp::lib::error_code ec;
                impl_->client.close(impl_->hdl,
                                    websocketpp::close::status::going_away,
                                    "client closing", ec);
                impl_->hdlSet = false;
            }
        }
        impl_->client.stop();
    } catch (...) {
        // Best-effort shutdown.
    }

    if (ioThread_.joinable()) {
        ioThread_.join();
    }
}

}  // namespace client
