#include "doctest.h"
#include "server/ConnectionRegistry.h"

#include <memory>
#include <string>

namespace {

ConnectionHdl makeHdl(std::shared_ptr<void>& alive) {
    alive = std::make_shared<int>(0);
    return alive;
}

}  // namespace

TEST_CASE("ConnectionRegistry registerConnection") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);

    const MatchQueue::PlayerId id = registry.registerConnection(hdl);
    CHECK(id >= 1);

    const ClientConn* client = registry.getClient(hdl);
    REQUIRE(client != nullptr);
    CHECK(client->state == ClientState::PendingAuth);
    CHECK(client->playerId == id);
    CHECK(client->username.empty());
    CHECK(client->rating == 0);
    CHECK(client->matchId == -1);
    CHECK(client->color == '?');

    const auto resolved = registry.hdlForPlayer(id);
    REQUIRE(resolved.has_value());
    CHECK_FALSE(resolved->owner_before(hdl));
    CHECK_FALSE(hdl.owner_before(*resolved));

    std::shared_ptr<void> alive2;
    ConnectionHdl hdl2 = makeHdl(alive2);
    const MatchQueue::PlayerId id2 = registry.registerConnection(hdl2);
    CHECK(id2 != id);
    CHECK(id2 > id);
}

TEST_CASE("ConnectionRegistry removeConnection") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);
    const MatchQueue::PlayerId id = registry.registerConnection(hdl);

    registry.removeConnection(hdl);
    CHECK(registry.getClient(hdl) == nullptr);
    CHECK_FALSE(registry.hdlForPlayer(id).has_value());

    std::shared_ptr<void> neverRegistered;
    ConnectionHdl ghost = makeHdl(neverRegistered);
    registry.removeConnection(ghost);
    CHECK(registry.getClient(ghost) == nullptr);
}

TEST_CASE("ConnectionRegistry setAuthenticated") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);
    registry.registerConnection(hdl);

    registry.setAuthenticated(hdl, "alice", 1500);
    const ClientConn* client = registry.getClient(hdl);
    REQUIRE(client != nullptr);
    CHECK(client->state == ClientState::Authenticated);
    CHECK(client->username == "alice");
    CHECK(client->rating == 1500);
}

TEST_CASE("ConnectionRegistry setViewer") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);
    registry.registerConnection(hdl);

    registry.setViewer(hdl, "bob", 1600);
    const ClientConn* client = registry.getClient(hdl);
    REQUIRE(client != nullptr);
    CHECK(client->state == ClientState::Viewer);
    CHECK(client->username == "bob");
    CHECK(client->rating == 1600);
}

TEST_CASE("ConnectionRegistry usernameAlreadyConnected") {
    ConnectionRegistry registry;
    std::shared_ptr<void> pendingAlive;
    std::shared_ptr<void> authAlive;
    ConnectionHdl pendingHdl = makeHdl(pendingAlive);
    ConnectionHdl authHdl = makeHdl(authAlive);

    registry.registerConnection(pendingHdl);
    registry.registerConnection(authHdl);
    // pendingHdl stays PendingAuth (skipped by usernameAlreadyConnected).
    registry.setAuthenticated(authHdl, "carol", 1400);

    CHECK(registry.usernameAlreadyConnected("carol"));
    CHECK_FALSE(registry.usernameAlreadyConnected("nobody"));
    CHECK_FALSE(registry.usernameAlreadyConnected(""));
    CHECK(registry.getClient(pendingHdl)->state == ClientState::PendingAuth);

    registry.setViewer(pendingHdl, "carol", 1000);
    // Duplicate username on a non-PendingAuth client still counts.
    CHECK(registry.usernameAlreadyConnected("carol"));
}

TEST_CASE("ConnectionRegistry seatedCount") {
    ConnectionRegistry registry;
    CHECK(registry.seatedCount() == 0);

    std::shared_ptr<void> a1, a2, a3, a4, a5;
    ConnectionHdl pending = makeHdl(a1);
    ConnectionHdl queued = makeHdl(a2);
    ConnectionHdl viewer = makeHdl(a3);
    ConnectionHdl seated1 = makeHdl(a4);
    ConnectionHdl seated2 = makeHdl(a5);

    registry.registerConnection(pending);
    registry.registerConnection(queued);
    registry.registerConnection(viewer);
    registry.registerConnection(seated1);
    registry.registerConnection(seated2);

    registry.setAuthenticated(queued, "q", 1000);
    registry.setQueued(queued);
    registry.setViewer(viewer, "v", 1000);
    registry.setAuthenticated(seated1, "s1", 1000);
    registry.setSeated(seated1, 1, 'W');

    CHECK(registry.seatedCount() == 1);

    registry.setAuthenticated(seated2, "s2", 1000);
    registry.setSeated(seated2, 1, 'B');
    CHECK(registry.seatedCount() == 2);

    std::shared_ptr<void> a6;
    ConnectionHdl seated3 = makeHdl(a6);
    registry.registerConnection(seated3);
    registry.setAuthenticated(seated3, "s3", 1000);
    registry.setSeated(seated3, 2, 'W');
    CHECK(registry.seatedCount() == 3);
}

TEST_CASE("ConnectionRegistry setQueued and setAuthenticatedFromQueue") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);
    registry.registerConnection(hdl);
    registry.setAuthenticated(hdl, "dave", 1550);

    registry.setQueued(hdl);
    REQUIRE(registry.getClient(hdl)->state == ClientState::Queued);

    registry.setAuthenticatedFromQueue(hdl);
    const ClientConn* client = registry.getClient(hdl);
    REQUIRE(client != nullptr);
    CHECK(client->state == ClientState::Authenticated);
    CHECK(client->matchId == -1);
    CHECK(client->color == '?');
    CHECK(client->username == "dave");
    CHECK(client->rating == 1550);
}

TEST_CASE("ConnectionRegistry setSeated and clearSeat") {
    ConnectionRegistry registry;
    std::shared_ptr<void> alive;
    ConnectionHdl hdl = makeHdl(alive);
    registry.registerConnection(hdl);
    registry.setAuthenticated(hdl, "erin", 1700);

    registry.setSeated(hdl, 7, 'B');
    const ClientConn* seated = registry.getClient(hdl);
    REQUIRE(seated != nullptr);
    CHECK(seated->state == ClientState::Seated);
    CHECK(seated->matchId == 7);
    CHECK(seated->color == 'B');

    registry.clearSeat(hdl);
    const ClientConn* cleared = registry.getClient(hdl);
    REQUIRE(cleared != nullptr);
    CHECK(cleared->state == ClientState::Authenticated);
    CHECK(cleared->matchId == -1);
    CHECK(cleared->color == '?');
    CHECK(cleared->username == "erin");
    CHECK(cleared->rating == 1700);
}

TEST_CASE("ConnectionRegistry two independent connections") {
    ConnectionRegistry registry;
    std::shared_ptr<void> aliveA;
    std::shared_ptr<void> aliveB;
    ConnectionHdl hdlA = makeHdl(aliveA);
    ConnectionHdl hdlB = makeHdl(aliveB);

    const MatchQueue::PlayerId idA = registry.registerConnection(hdlA);
    const MatchQueue::PlayerId idB = registry.registerConnection(hdlB);
    CHECK(idA != idB);

    registry.setAuthenticated(hdlA, "alpha", 1100);
    registry.setViewer(hdlB, "beta", 2200);

    const ClientConn* a = registry.getClient(hdlA);
    const ClientConn* b = registry.getClient(hdlB);
    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);
    CHECK(a->playerId == idA);
    CHECK(b->playerId == idB);
    CHECK(a->state == ClientState::Authenticated);
    CHECK(b->state == ClientState::Viewer);
    CHECK(a->username == "alpha");
    CHECK(b->username == "beta");
    CHECK(a->rating == 1100);
    CHECK(b->rating == 2200);

    registry.setSeated(hdlA, 3, 'W');
    CHECK(registry.getClient(hdlB)->state == ClientState::Viewer);
    CHECK(registry.getClient(hdlB)->matchId == -1);

    const auto resolvedA = registry.hdlForPlayer(idA);
    const auto resolvedB = registry.hdlForPlayer(idB);
    REQUIRE(resolvedA.has_value());
    REQUIRE(resolvedB.has_value());
    CHECK_FALSE(resolvedA->owner_before(hdlA));
    CHECK_FALSE(hdlA.owner_before(*resolvedA));
    CHECK_FALSE(resolvedB->owner_before(hdlB));
    CHECK_FALSE(hdlB.owner_before(*resolvedB));
}
