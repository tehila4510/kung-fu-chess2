#ifndef REMOTE_STATE_SOURCE_H
#define REMOTE_STATE_SOURCE_H

#include "client/InboundMessageQueue.h"
#include "graphics/IFrameStateSource.h"
#include "model/GameState.h"
#include "realtime/MotionView.h"
#include "realtime/RestView.h"

#include <string>
#include <vector>

namespace client {

enum class MatchPhase {
    Lobby,
    Searching,
    Seated,
    Disconnected
};

// Online frame source: drains the inbound queue inside advance() and caches DTOs.
class RemoteStateSource : public graphics::IFrameStateSource {
public:
    explicit RemoteStateSource(InboundMessageQueue& inbound);

    void setIdentity(const std::string& username, int rating);

    void advance(int dtMs) override;
    GameSnapshot getSnapshot() const override;
    std::vector<MotionView> activeMotions() const override;
    std::vector<RestView> activeRests() const override;
    bool isGameOver() const override;

    MatchPhase phase() const;
    char assignedColor() const;
    const std::string& username() const;
    int rating() const;
    const std::string& statusLine() const;
    const std::string& bannerText() const;
    bool isPlayVisible() const;
    void notePlayRequested();

private:
    void applyMessage(const std::string& raw);
    void enterLobbyStatus();
    void clearBoardCache();

    InboundMessageQueue& inbound_;
    std::string username_;
    int rating_ = 0;
    MatchPhase phase_ = MatchPhase::Lobby;
    char assignedColor_ = '?';
    std::string statusLine_;
    std::string bannerText_;

    GameSnapshot snapshot_;
    std::vector<MotionView> motions_;
    std::vector<RestView> rests_;
};

}  // namespace client

#endif
