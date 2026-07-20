#include "auth/AuthController.h"

namespace {

bool startsWith(const std::string& text, const std::string& prefix) {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

}  // namespace

AuthController::AuthController(UserService& users) : users_(users) {}

AuthCommandResult AuthController::handle(const std::string& line) {
    AuthCommandResult result;
    if (!startsWith(line, "AUTH ")) {
        result.reason = "auth_required";
        return result;
    }

    const std::string rest = line.substr(5);
    const auto space = rest.find(' ');
    if (space == std::string::npos || space == 0 || space + 1 >= rest.size()) {
        result.reason = "invalid_auth";
        return result;
    }

    const std::string username = rest.substr(0, space);
    const std::string password = rest.substr(space + 1);

    const AuthResult auth = users_.loginOrRegister(username, password);
    result.ok = auth.ok;
    result.reason = auth.reason;
    result.username = username;
    result.rating = auth.rating;
    return result;
}
