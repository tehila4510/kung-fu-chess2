#ifndef USER_SERVICE_H
#define USER_SERVICE_H

#include "auth/AuthResult.h"
#include "auth/UserRepository.h"

#include <string>

// Business rules for users: auth and ELO updates. No SQL, no wire parsing.
class UserService {
    UserRepository& repo_;

public:
    explicit UserService(UserRepository& repo);

    // Unknown username -> create with rating 1200. Known -> verify password.
    AuthResult loginOrRegister(const std::string& username, const std::string& password);

    void applyGameResult(const std::string& whiteUser, const std::string& blackUser,
                         char winnerColor);
};

#endif
