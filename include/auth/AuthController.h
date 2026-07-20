#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "auth/UserService.h"

#include <string>

struct AuthCommandResult {
    bool ok = false;
    std::string reason;
    std::string username;
    int rating = 0;
};

// Parses AUTH wire lines and delegates to UserService. No JSON / WebSocket.
class AuthController {
    UserService& users_;

public:
    explicit AuthController(UserService& users);
    AuthCommandResult handle(const std::string& line);
};

#endif
