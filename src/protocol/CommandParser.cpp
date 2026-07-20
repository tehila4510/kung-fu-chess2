#include "protocol/CommandParser.h"

#include "protocol/Algebraic.h"

#include <cctype>
#include <sstream>

namespace protocol {
namespace {

std::string trim(const std::string& s) {
    const size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) {
        return "";
    }
    const size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

char normalizeColor(char c) {
    const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    if (upper == 'W' || upper == 'B') {
        return upper;
    }
    return '?';
}

}  // namespace

ParseCommandResult parseWireCommand(const std::string& line, int rowCount,
                                    int columnCount) {
    ParseCommandResult result;
    result.command.raw = line;
    const std::string text = trim(line);
    if (text.empty()) {
        result.error = "empty_command";
        return result;
    }

    std::string upper;
    upper.reserve(text.size());
    for (char c : text) {
        upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    if (upper == "STATE") {
        result.ok = true;
        result.command.kind = WireCommandKind::State;
        return result;
    }

    if (upper.rfind("WAIT", 0) == 0) {
        std::istringstream iss(upper);
        std::string keyword;
        int ms = 0;
        if (!(iss >> keyword >> ms) || keyword != "WAIT" || ms < 0) {
            result.error = "invalid_wait";
            return result;
        }
        result.ok = true;
        result.command.kind = WireCommandKind::Wait;
        result.command.waitMs = ms;
        return result;
    }

    if (upper.size() >= 4 && (upper[1] == 'J')) {
        const char color = normalizeColor(upper[0]);
        if (color == '?' || upper.size() != 4) {
            result.error = "invalid_jump";
            return result;
        }
        const std::string square = text.substr(2, 2);
        const auto at = squareToPosition(square, rowCount, columnCount);
        if (!at.has_value()) {
            result.error = "invalid_square";
            return result;
        }
        result.ok = true;
        result.command.kind = WireCommandKind::Jump;
        result.command.color = color;
        result.command.from = *at;
        result.command.to = *at;
        return result;
    }

    if (upper.size() >= 6 && upper[1] == 'M') {
        const char color = normalizeColor(upper[0]);
        if (color == '?' || upper.size() != 6) {
            result.error = "invalid_move";
            return result;
        }
        const std::string fromSq = text.substr(2, 2);
        const std::string toSq = text.substr(4, 2);
        const auto from = squareToPosition(fromSq, rowCount, columnCount);
        const auto to = squareToPosition(toSq, rowCount, columnCount);
        if (!from.has_value() || !to.has_value()) {
            result.error = "invalid_square";
            return result;
        }
        result.ok = true;
        result.command.kind = WireCommandKind::Move;
        result.command.color = color;
        result.command.from = *from;
        result.command.to = *to;
        return result;
    }

    result.error = "unknown_command";
    return result;
}

}  // namespace protocol
