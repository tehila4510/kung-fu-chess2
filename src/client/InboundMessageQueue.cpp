#include "client/InboundMessageQueue.h"

#include <chrono>
#include <utility>

namespace client {

void InboundMessageQueue::push(std::string message) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(message));
    }
    cv_.notify_all();
}

std::vector<std::string> InboundMessageQueue::drain() {
    std::vector<std::string> out;
    std::lock_guard<std::mutex> lock(mutex_);
    while (!queue_.empty()) {
        out.push_back(std::move(queue_.front()));
        queue_.pop();
    }
    return out;
}

std::optional<std::string> InboundMessageQueue::waitForMessage(int timeoutMs) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (timeoutMs < 0) {
        cv_.wait(lock, [this] { return !queue_.empty(); });
    } else {
        const bool ready = cv_.wait_for(
            lock, std::chrono::milliseconds(timeoutMs),
            [this] { return !queue_.empty(); });
        if (!ready) {
            return std::nullopt;
        }
    }
    std::string message = std::move(queue_.front());
    queue_.pop();
    return message;
}

}  // namespace client
