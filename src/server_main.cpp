#include "server/WebSocketServer.h"

#include <iostream>

int main() {
    try {
        WebSocketServer server;
        return server.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << '\n';
        return 1;
    }
}
