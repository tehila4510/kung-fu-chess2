#ifndef MATCH_COORDINATOR_H
#define MATCH_COORDINATOR_H

#include "server/ConnectionRegistry.h"
#include "server/MatchQueue.h"

#include <string>
#include <vector>

struct MatchPlayResult {
    enum class Kind { Rejected, Searching, Matched } kind = Kind::Rejected;
    std::string rejectReason;
    ConnectionHdl white;
    ConnectionHdl black;
};

struct MatchExpireEvent {
    ConnectionHdl hdl;
};

// Owns MatchQueue; mutates Queued / Authenticated via ConnectionRegistry.
// Never sends JSON; never sets Seated.
class MatchCoordinator {
    ConnectionRegistry& registry_;
    MatchQueue queue_;

public:
    explicit MatchCoordinator(ConnectionRegistry& registry);

    void removeFromQueue(MatchQueue::PlayerId id);
    MatchPlayResult play(ConnectionHdl hdl);
    std::vector<MatchExpireEvent> expire(MatchQueue::TimePoint now);
};

#endif
