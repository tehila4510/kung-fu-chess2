#ifndef INBOUND_MESSAGE_QUEUE_H
#define INBOUND_MESSAGE_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace client {

// Thread-safe queue: WebSocket I/O thread pushes; main thread drains.
class InboundMessageQueue {
public:
    void push(std::string message);
    std::vector<std::string> drain();
    // Blocks until a message is available or timeoutMs elapses. Returns nullopt on timeout.
    std::optional<std::string> waitForMessage(int timeoutMs);

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::string> queue_;
};

}  // namespace client

#endif
