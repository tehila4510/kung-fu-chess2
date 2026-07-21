#include "doctest.h"
#include "server/ConnectionRegistry.h"
#include "server/MatchCoordinator.h"
#include "server/MatchQueue.h"

#include <chrono>
#include <memory>

using Clock = std::chrono::steady_clock;
using Ms = std::chrono::milliseconds;

namespace {

ConnectionHdl makeHdl(std::shared_ptr<void>& alive) {
    alive = std::make_shared<int>(0);
    return alive;
}

bool sameHdl(ConnectionHdl a, ConnectionHdl b) {
    return !a.owner_before(b) && !b.owner_before(a);
}

ConnectionHdl registerAuth(ConnectionRegistry& registry,
                           std::shared_ptr<void>& alive,
                           const std::string& username, int rating) {
    ConnectionHdl hdl = makeHdl(alive);
    registry.registerConnection(hdl);
    registry.setAuthenticated(hdl, username, rating);
    return hdl;
}

}  // namespace

TEST_CASE("MatchCoordinator play rejects invalid states") {
    ConnectionRegistry registry;
    MatchCoordinator coordinator(registry);

    SUBCASE("missing connection") {
        std::shared_ptr<void> alive;
        ConnectionHdl ghost = makeHdl(alive);
        const MatchPlayResult result = coordinator.play(ghost);
        CHECK(result.kind == MatchPlayResult::Kind::Rejected);
        CHECK(result.rejectReason == "not_authenticated");
    }

    SUBCASE("PendingAuth") {
        std::shared_ptr<void> alive;
        ConnectionHdl hdl = makeHdl(alive);
        registry.registerConnection(hdl);
        const MatchPlayResult result = coordinator.play(hdl);
        CHECK(result.kind == MatchPlayResult::Kind::Rejected);
        CHECK(result.rejectReason == "not_authenticated");
    }

    SUBCASE("Viewer") {
        std::shared_ptr<void> alive;
        ConnectionHdl hdl = makeHdl(alive);
        registry.registerConnection(hdl);
        registry.setViewer(hdl, "v", 1500);
        const MatchPlayResult result = coordinator.play(hdl);
        CHECK(result.kind == MatchPlayResult::Kind::Rejected);
        CHECK(result.rejectReason == "viewer_cannot_play");
    }

    SUBCASE("already Queued") {
        std::shared_ptr<void> alive;
        ConnectionHdl hdl = registerAuth(registry, alive, "q", 1500);
        REQUIRE(coordinator.play(hdl).kind == MatchPlayResult::Kind::Searching);
        const MatchPlayResult again = coordinator.play(hdl);
        CHECK(again.kind == MatchPlayResult::Kind::Rejected);
        CHECK(again.rejectReason == "already_searching");
    }
}

TEST_CASE("MatchCoordinator play alone yields Searching") {
    ConnectionRegistry registry;
    MatchCoordinator coordinator(registry);
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = registerAuth(registry, alive, "solo", 1500);

    const MatchPlayResult result = coordinator.play(hdl);
    CHECK(result.kind == MatchPlayResult::Kind::Searching);
    REQUIRE(registry.getClient(hdl) != nullptr);
    CHECK(registry.getClient(hdl)->state == ClientState::Queued);
}

TEST_CASE("MatchCoordinator play matches two compatible waiters") {
    ConnectionRegistry registry;
    MatchCoordinator coordinator(registry);
    std::shared_ptr<void> aliveW;
    std::shared_ptr<void> aliveB;
    ConnectionHdl first = registerAuth(registry, aliveW, "white_waiter", 1500);
    ConnectionHdl second = registerAuth(registry, aliveB, "black_joiner", 1550);

    REQUIRE(coordinator.play(first).kind == MatchPlayResult::Kind::Searching);
    const MatchPlayResult result = coordinator.play(second);
    REQUIRE(result.kind == MatchPlayResult::Kind::Matched);
    CHECK(sameHdl(result.white, first));
    CHECK(sameHdl(result.black, second));

    // MatchCoordinator must not seat anyone.
    CHECK(registry.getClient(first)->state == ClientState::Queued);
    CHECK(registry.getClient(second)->state == ClientState::Queued);
    CHECK(registry.getClient(first)->matchId == -1);
    CHECK(registry.getClient(second)->matchId == -1);
    CHECK(registry.getClient(first)->color == '?');
    CHECK(registry.getClient(second)->color == '?');
}

TEST_CASE("MatchCoordinator expire after timeout") {
    ConnectionRegistry registry;
    MatchCoordinator coordinator(registry);
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = registerAuth(registry, alive, "late", 1500);

    REQUIRE(coordinator.play(hdl).kind == MatchPlayResult::Kind::Searching);
    CHECK(registry.getClient(hdl)->state == ClientState::Queued);

    const auto events =
        coordinator.expire(Clock::now() + Ms(MatchQueue::kQueueTimeoutMs + 1));
    REQUIRE(events.size() == 1);
    CHECK(sameHdl(events[0].hdl, hdl));
    CHECK(registry.getClient(hdl)->state == ClientState::Authenticated);
    CHECK(registry.getClient(hdl)->matchId == -1);
    CHECK(registry.getClient(hdl)->color == '?');
}

TEST_CASE("MatchCoordinator removeFromQueue drops waiter") {
    ConnectionRegistry registry;
    MatchCoordinator coordinator(registry);
    std::shared_ptr<void> aliveA;
    std::shared_ptr<void> aliveB;
    ConnectionHdl first = registerAuth(registry, aliveA, "gone", 1500);
    ConnectionHdl second = registerAuth(registry, aliveB, "later", 1500);

    REQUIRE(coordinator.play(first).kind == MatchPlayResult::Kind::Searching);
    const MatchQueue::PlayerId firstId = registry.getClient(first)->playerId;
    coordinator.removeFromQueue(firstId);

    // Removed id must not expire or match.
    const auto events =
        coordinator.expire(Clock::now() + Ms(MatchQueue::kQueueTimeoutMs + 1));
    CHECK(events.empty());
    CHECK(registry.getClient(first)->state == ClientState::Queued);

    const MatchPlayResult result = coordinator.play(second);
    CHECK(result.kind == MatchPlayResult::Kind::Searching);
    CHECK(registry.getClient(second)->state == ClientState::Queued);
}
