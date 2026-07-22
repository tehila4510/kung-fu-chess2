#include "client/SessionBootstrap.h"

#include "protocol/StateDeserializer.h"

#include <string>

namespace client {
namespace {

bool startsWith(const std::string& line, const char* prefix) {
    const std::size_t n = std::char_traits<char>::length(prefix);
    return line.size() >= n && line.compare(0, n, prefix) == 0;
}

}  // namespace

BootstrapResult SessionBootstrap::run(WebSocketClient& client,
                                      const std::string& host,
                                      std::uint16_t port, std::istream& in,
                                      std::ostream& out) {
    BootstrapResult result;

    try {
        out << "Connecting to ws://" << host << ":" << port << "...\n";
        client.connect(host, port);
    } catch (const std::exception& ex) {
        result.error = ex.what();
        return result;
    }

    // Wait for auth_required (ignore unrelated frames).
    bool sawAuthRequired = false;
    const auto deadlineWait = [&](int timeoutMs) -> protocol::RemoteMessage {
        const auto raw = client.inboundQueue().waitForMessage(timeoutMs);
        if (!raw) {
            return protocol::RemoteMessage{};
        }
        return protocol::deserializeRemoteMessage(*raw);
    };

    {
        const int stepMs = 5000;
        int waited = 0;
        while (waited < kAuthTimeoutMs) {
            const protocol::RemoteMessage msg = deadlineWait(stepMs);
            waited += stepMs;
            if (msg.kind == protocol::RemoteMessageKind::AuthRequired) {
                sawAuthRequired = true;
                break;
            }
            if (msg.kind == protocol::RemoteMessageKind::Disconnected) {
                result.error = "Disconnected before AUTH: " + msg.reason;
                return result;
            }
            if (msg.kind == protocol::RemoteMessageKind::Unknown &&
                !client.isConnected()) {
                result.error = "Connection lost before AUTH.";
                return result;
            }
        }
    }

    if (!sawAuthRequired) {
        result.error = "Timed out waiting for auth_required.";
        return result;
    }

    out << "Server requires AUTH. Enter: AUTH <username> <password>\n";

    while (true) {
        out << "> " << std::flush;
        std::string line;
        if (!std::getline(in, line)) {
            result.error = "STDIN closed before AUTH completed.";
            return result;
        }
        if (line.empty()) {
            continue;
        }
        if (!startsWith(line, "AUTH ")) {
            out << "Expected AUTH <username> <password>\n";
            continue;
        }

        client.send(line);

        bool done = false;
        const int stepMs = 5000;
        int waited = 0;
        while (waited < kAuthTimeoutMs && !done) {
            const protocol::RemoteMessage msg = deadlineWait(stepMs);
            waited += stepMs;

            if (msg.kind == protocol::RemoteMessageKind::AuthOk) {
                result.ok = true;
                result.username = msg.authOk.username;
                result.rating = msg.authOk.rating;
                result.role = msg.authOk.role;
                out << "Authenticated as " << result.username << " (rating "
                    << result.rating << ", role " << result.role << ")\n";
                return result;
            }
            if (msg.kind == protocol::RemoteMessageKind::Error) {
                out << "AUTH failed: " << msg.reason
                    << ". Try again (AUTH <user> <pass>).\n";
                done = true;
                break;
            }
            if (msg.kind == protocol::RemoteMessageKind::Disconnected) {
                result.error = "Disconnected during AUTH: " + msg.reason;
                return result;
            }
        }
        if (!done) {
            result.error = "Timed out waiting for auth_ok.";
            return result;
        }
    }
}

}  // namespace client
