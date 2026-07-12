#include "io/BoardParser.h"

#include <istream>
#include <sstream>

namespace {

std::string trim(const std::string& s) {
    const size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    const size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool isValidToken(const std::string& tok) {
    if (tok == ".") return true;
    if (tok.size() != 2) return false;
    if (tok[0] != 'w' && tok[0] != 'b') return false;
    static const std::string pieces = "KQRBNP";
    return pieces.find(tok[1]) != std::string::npos;
}

// Split a trimmed line into whitespace-separated cell tokens.
std::vector<std::string> tokenize(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

// Validate token grammar and rectangularity, then hand ownership to a Board.
BoardParseResult finalize(std::vector<std::vector<std::string>> grid) {
    if (grid.empty()) {
        return { std::nullopt, BoardParseStatus::EmptyBoard };
    }
    const size_t width = grid.front().size();
    for (const auto& row : grid) {
        if (row.size() != width) {
            return { std::nullopt, BoardParseStatus::RowWidthMismatch };
        }
    }
    return { Board(std::move(grid)), BoardParseStatus::Ok };
}

} // namespace

const char* toReasonCode(BoardParseStatus status) {
    switch (status) {
        case BoardParseStatus::UnknownToken:     return "ERROR UNKNOWN_TOKEN";
        case BoardParseStatus::RowWidthMismatch: return "ERROR ROW_WIDTH_MISMATCH";
        case BoardParseStatus::EmptyBoard:       return "ERROR EMPTY_BOARD";
        case BoardParseStatus::Ok:               return "";
    }
    return "";
}

BoardParseResult BoardParser::parse(std::istream& input) const {
    std::vector<std::vector<std::string>> grid;
    std::string line;
    while (std::getline(input, line)) {
        const std::string t = trim(line);
        if (t == "Commands:") break;
        if (t.empty() || t == "Board:") continue;

        std::vector<std::string> tokens = tokenize(t);
        for (const std::string& tk : tokens) {
            if (!isValidToken(tk)) {
                return { std::nullopt, BoardParseStatus::UnknownToken };
            }
        }
        grid.push_back(std::move(tokens));
    }
    return finalize(std::move(grid));
}

BoardParseResult BoardParser::parseText(const std::string& text) const {
    std::istringstream iss(text);
    return parse(iss);
}

BoardParseResult BoardParser::parseLines(const std::vector<std::string>& lines, size_t& index) const {
    while (index < lines.size() && trim(lines[index]) != "Board:") ++index;
    if (index < lines.size()) ++index; // consume the "Board:" marker

    std::vector<std::vector<std::string>> grid;
    while (index < lines.size()) {
        const std::string t = trim(lines[index]);
        if (t == "Commands:") { ++index; break; }
        if (t.empty()) { ++index; continue; }

        std::vector<std::string> tokens = tokenize(t);
        for (const std::string& tk : tokens) {
            if (!isValidToken(tk)) {
                return { std::nullopt, BoardParseStatus::UnknownToken };
            }
        }
        grid.push_back(std::move(tokens));
        ++index;
    }
    return finalize(std::move(grid));
}
