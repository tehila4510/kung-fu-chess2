#ifndef PROTOCOL_COMMAND_PARSER_H
#define PROTOCOL_COMMAND_PARSER_H

#include "model/Position.h"

#include <optional>
#include <string>

namespace protocol {

enum class WireCommandKind {
    Move,
    Jump,
    Wait,
    State
};

struct WireCommand {
    WireCommandKind kind = WireCommandKind::State;
    char color = '?';
    Position from;
    Position to;
    int waitMs = 0;
    std::string raw;
};

struct ParseCommandResult {
    bool ok = false;
    std::string error;
    WireCommand command;
};

// Client -> server plain text: WMe2e4, WJe2, WAIT 100, STATE
ParseCommandResult parseWireCommand(const std::string& line, int rowCount,
                                    int columnCount);

}  // namespace protocol

#endif
