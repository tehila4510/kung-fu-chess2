#ifndef SESSION_BOOTSTRAP_H
#define SESSION_BOOTSTRAP_H

#include "client/WebSocketClient.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace client {

struct BootstrapResult {
    bool ok = false;
    std::string username;
    int rating = 0;
    std::string role;
    std::string error;
};

// Blocking shell AUTH before any OpenCV window. Uses AUTH <user> <pass> lines.
class SessionBootstrap {
public:
    static constexpr int kAuthTimeoutMs = 60000;

    BootstrapResult run(WebSocketClient& client, const std::string& host,
                        std::uint16_t port, std::istream& in, std::ostream& out);
};

}  // namespace client

#endif
