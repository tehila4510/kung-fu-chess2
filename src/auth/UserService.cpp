#include "auth/UserService.h"

#include "auth/Elo.h"
#include "auth/PasswordHasher.h"

#include <iostream>
#include <stdexcept>

UserService::UserService(UserRepository& repo) : repo_(repo) {}

AuthResult UserService::loginOrRegister(const std::string& username,
                                        const std::string& password) {
    AuthResult result;
    if (username.empty() || password.empty()) {
        result.reason = "invalid_auth";
        return result;
    }

    std::string storedHash;
    int rating = elo::kInitialRating;
    if (repo_.findByUsername(username, storedHash, rating)) {
        if (!PasswordHasher::verify(password, storedHash)) {
            result.reason = "bad_password";
            return result;
        }
        result.ok = true;
        result.reason = "ok";
        result.rating = rating;
        return result;
    }

    const std::string hash = PasswordHasher::hash(password);
    repo_.insertUser(username, hash, elo::kInitialRating);

    result.ok = true;
    result.reason = "registered";
    result.rating = elo::kInitialRating;
    return result;
}

void UserService::applyGameResult(const std::string& whiteUser,
                                  const std::string& blackUser, char winnerColor) {
    if (winnerColor != 'W' && winnerColor != 'B') {
        throw std::invalid_argument("winnerColor must be W or B");
    }

    const int ratingW = repo_.getRating(whiteUser);
    const int ratingB = repo_.getRating(blackUser);
    const double scoreW = (winnerColor == 'W') ? 1.0 : 0.0;
    const double scoreB = 1.0 - scoreW;

    const int newW = elo::updateRating(ratingW, ratingB, scoreW);
    const int newB = elo::updateRating(ratingB, ratingW, scoreB);
    repo_.setRating(whiteUser, newW);
    repo_.setRating(blackUser, newB);

    std::cout << "ELO update: " << whiteUser << " " << ratingW << "->" << newW
              << " | " << blackUser << " " << ratingB << "->" << newB
              << " (winner " << winnerColor << ")\n";
}
