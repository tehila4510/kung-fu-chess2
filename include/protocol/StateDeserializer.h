#ifndef PROTOCOL_STATE_DESERIALIZER_H
#define PROTOCOL_STATE_DESERIALIZER_H

#include "bus/GameEvent.h"
#include "model/GameState.h"
#include "realtime/MotionView.h"
#include "realtime/RestView.h"

#include <optional>
#include <string>
#include <vector>

namespace protocol {

enum class RemoteMessageKind {
    AuthRequired,
    AuthOk,
    Error,
    Searching,
    Welcome,
    State,
    Disconnected,
    Unknown
};

struct AuthOkPayload {
    std::string username;
    int rating = 0;
    std::string role;
};

struct WelcomePayload {
    char color = '?';
    std::string message;
};

struct RemoteStatePayload {
    std::string status;
    std::string reason;
    bool gameOver = false;
    GameSnapshot snapshot;
    std::vector<MotionView> motions;
    std::vector<RestView> rests;
    std::vector<GameEvent> events;
};

struct RemoteMessage {
    RemoteMessageKind kind = RemoteMessageKind::Unknown;
    std::string reason;
    AuthOkPayload authOk;
    WelcomePayload welcome;
    RemoteStatePayload state;
};

// Parse one inbound JSON text frame. Never throws for malformed JSON — returns Unknown.
RemoteMessage deserializeRemoteMessage(const std::string& jsonText);

}  // namespace protocol

#endif
