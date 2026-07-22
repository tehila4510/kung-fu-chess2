#include "protocol/StateDeserializer.h"

#include "protocol/Algebraic.h"

#include <nlohmann/json.hpp>

namespace protocol {
namespace {

GameEventType eventTypeFromString(const std::string& name) {
    if (name == "MoveMade") {
        return GameEventType::MoveMade;
    }
    if (name == "JumpMade") {
        return GameEventType::JumpMade;
    }
    if (name == "PieceCaptured") {
        return GameEventType::PieceCaptured;
    }
    if (name == "PiecePromoted") {
        return GameEventType::PiecePromoted;
    }
    if (name == "PieceSelected") {
        return GameEventType::PieceSelected;
    }
    if (name == "SelectionCleared") {
        return GameEventType::SelectionCleared;
    }
    if (name == "GameEnded") {
        return GameEventType::GameEnded;
    }
    if (name == "ScoreUpdated") {
        return GameEventType::ScoreUpdated;
    }
    if (name == "GameStarted") {
        return GameEventType::GameStarted;
    }
    return GameEventType::MoveMade;
}

RestKind restKindFromString(const std::string& kind) {
    return kind == "short" ? RestKind::Short : RestKind::Long;
}

char colorFromJson(const nlohmann::json& value) {
    if (value.is_string() && !value.get<std::string>().empty()) {
        return value.get<std::string>()[0];
    }
    return '?';
}

GameEvent parseEvent(const nlohmann::json& j) {
    GameEvent event;
    if (j.contains("type") && j["type"].is_string()) {
        event.type = eventTypeFromString(j["type"].get<std::string>());
    }
    if (j.contains("color")) {
        event.color = colorFromJson(j["color"]);
    }
    if (j.contains("from") && j["from"].is_string()) {
        event.from = j["from"].get<std::string>();
    }
    if (j.contains("to") && j["to"].is_string()) {
        event.to = j["to"].get<std::string>();
    }
    if (j.contains("piece") && j["piece"].is_string()) {
        event.piece = j["piece"].get<std::string>();
    }
    if (j.contains("capturedPiece") && j["capturedPiece"].is_string()) {
        event.capturedPiece = j["capturedPiece"].get<std::string>();
    }
    if (j.contains("whiteScore") && j["whiteScore"].is_number_integer()) {
        event.whiteScore = j["whiteScore"].get<int>();
    }
    if (j.contains("blackScore") && j["blackScore"].is_number_integer()) {
        event.blackScore = j["blackScore"].get<int>();
    }
    if (j.contains("reason") && j["reason"].is_string()) {
        event.reason = j["reason"].get<std::string>();
    }
    return event;
}

RemoteStatePayload parseState(const nlohmann::json& root) {
    RemoteStatePayload state;
    if (root.contains("status") && root["status"].is_string()) {
        state.status = root["status"].get<std::string>();
    }
    if (root.contains("reason") && root["reason"].is_string()) {
        state.reason = root["reason"].get<std::string>();
    }
    if (root.contains("gameOver") && root["gameOver"].is_boolean()) {
        state.gameOver = root["gameOver"].get<bool>();
    }

    if (root.contains("board") && root["board"].is_array()) {
        for (const auto& rowJson : root["board"]) {
            std::vector<std::string> row;
            if (rowJson.is_array()) {
                for (const auto& cell : rowJson) {
                    if (cell.is_string()) {
                        row.push_back(cell.get<std::string>());
                    } else {
                        row.push_back(".");
                    }
                }
            }
            state.snapshot.cells.push_back(std::move(row));
        }
    }
    state.snapshot.gameOver = state.gameOver;

    const int rows = static_cast<int>(state.snapshot.cells.size());
    const int cols =
        rows > 0 ? static_cast<int>(state.snapshot.cells[0].size()) : 0;

    if (root.contains("motions") && root["motions"].is_array()) {
        for (const auto& motionJson : root["motions"]) {
            MotionView motion;
            if (motionJson.contains("piece") && motionJson["piece"].is_string()) {
                motion.piece = motionJson["piece"].get<std::string>();
            }
            if (motionJson.contains("progress") &&
                motionJson["progress"].is_number()) {
                motion.progress = motionJson["progress"].get<double>();
            }
            if (motionJson.contains("from") && motionJson["from"].is_string()) {
                const auto from = squareToPosition(
                    motionJson["from"].get<std::string>(), rows, cols);
                if (from) {
                    motion.from = *from;
                }
            }
            if (motionJson.contains("to") && motionJson["to"].is_string()) {
                const auto to = squareToPosition(
                    motionJson["to"].get<std::string>(), rows, cols);
                if (to) {
                    motion.to = *to;
                }
            }
            state.motions.push_back(std::move(motion));
        }
    }

    if (root.contains("rests") && root["rests"].is_array()) {
        for (const auto& restJson : root["rests"]) {
            RestView rest;
            if (restJson.contains("piece") && restJson["piece"].is_string()) {
                rest.piece = restJson["piece"].get<std::string>();
            }
            if (restJson.contains("kind") && restJson["kind"].is_string()) {
                rest.kind = restKindFromString(restJson["kind"].get<std::string>());
            }
            if (restJson.contains("remaining") &&
                restJson["remaining"].is_number()) {
                rest.remaining = restJson["remaining"].get<double>();
            }
            if (restJson.contains("at") && restJson["at"].is_string()) {
                const auto at = squareToPosition(restJson["at"].get<std::string>(),
                                                 rows, cols);
                if (at) {
                    rest.at = *at;
                }
            }
            state.rests.push_back(std::move(rest));
        }
    }

    if (root.contains("events") && root["events"].is_array()) {
        for (const auto& eventJson : root["events"]) {
            state.events.push_back(parseEvent(eventJson));
        }
    }

    return state;
}

}  // namespace

RemoteMessage deserializeRemoteMessage(const std::string& jsonText) {
    RemoteMessage message;
    try {
        const nlohmann::json root = nlohmann::json::parse(jsonText);
        if (!root.contains("type") || !root["type"].is_string()) {
            return message;
        }
        const std::string type = root["type"].get<std::string>();

        if (type == "auth_required") {
            message.kind = RemoteMessageKind::AuthRequired;
            return message;
        }
        if (type == "auth_ok") {
            message.kind = RemoteMessageKind::AuthOk;
            if (root.contains("username") && root["username"].is_string()) {
                message.authOk.username = root["username"].get<std::string>();
            }
            if (root.contains("rating") && root["rating"].is_number_integer()) {
                message.authOk.rating = root["rating"].get<int>();
            }
            if (root.contains("role") && root["role"].is_string()) {
                message.authOk.role = root["role"].get<std::string>();
            }
            return message;
        }
        if (type == "error") {
            message.kind = RemoteMessageKind::Error;
            if (root.contains("reason") && root["reason"].is_string()) {
                message.reason = root["reason"].get<std::string>();
            }
            return message;
        }
        if (type == "searching") {
            message.kind = RemoteMessageKind::Searching;
            return message;
        }
        if (type == "welcome") {
            message.kind = RemoteMessageKind::Welcome;
            message.welcome.color = colorFromJson(root.value("color", "?"));
            if (root.contains("message") && root["message"].is_string()) {
                message.welcome.message = root["message"].get<std::string>();
            }
            return message;
        }
        if (type == "state") {
            message.kind = RemoteMessageKind::State;
            message.state = parseState(root);
            return message;
        }
        if (type == "disconnected") {
            message.kind = RemoteMessageKind::Disconnected;
            if (root.contains("reason") && root["reason"].is_string()) {
                message.reason = root["reason"].get<std::string>();
            }
            return message;
        }
    } catch (...) {
        message.kind = RemoteMessageKind::Unknown;
    }
    return message;
}

}  // namespace protocol
