#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

#include <string>

// Thin wrapper around vendored bcrypt (third_party/bcrypt).
// hash() returns the full bcrypt string (salt embedded); verify() checks it.
class PasswordHasher {
public:
    static std::string hash(const std::string& password);
    static bool verify(const std::string& password, const std::string& storedHash);
};

#endif
