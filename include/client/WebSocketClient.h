#ifndef WEB_SOCKET_CLIENT_H
#define WEB_SOCKET_CLIENT_H

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#ifndef _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_STL_
#endif
#ifndef _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_THREAD_
#endif

#include "client/InboundMessageQueue.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>

namespace client {

// Persistent WebSocket client. I/O runs on a dedicated background thread.
// The render loop must never block on network I/O after connect/AUTH bootstrap.
class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;

    // Starts the I/O thread and opens ws://host:port. Blocks until open or failure.
    // Throws std::runtime_error on failure.
    void connect(const std::string& host, std::uint16_t port);

    // Thread-safe: posts a text frame send onto the I/O strand (non-blocking for caller).
    void send(const std::string& line);

    InboundMessageQueue& inboundQueue();
    const InboundMessageQueue& inboundQueue() const;

    bool isConnected() const;
    // True after the remote side closed or a transport error (sticky until reconnect).
    bool wasDisconnected() const;

    void close();

private:
    struct Impl;

    std::unique_ptr<Impl> impl_;
    InboundMessageQueue inbound_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> disconnected_{false};
    std::thread ioThread_;
};

}  // namespace client

#endif
