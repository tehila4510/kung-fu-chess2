#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Game.h"

static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        lines.push_back(line);
    }

    Game game;
    size_t index = 0;
    if (!game.setup(lines, index)) {
        return 0;
    }

    for (; index < lines.size(); index++) {
        std::string cleanLine = trim(lines[index]);
        if (cleanLine.empty()) continue;

        if (cleanLine == "print board") {
            game.handlePrintBoard();
            continue;
        }

        std::istringstream iss(cleanLine);
        std::string cmd;
        iss >> cmd;

        if (cmd == "click") {
            int x, y;
            if (iss >> x >> y) {
                game.handleClick(x, y);
            }
        } else if (cmd == "wait") {
            int ms;
            if (iss >> ms) {
                game.handleWait(ms);
            }
        }
    }

    return 0;
}


