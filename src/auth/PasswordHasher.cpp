#include "auth/PasswordHasher.h"

#include <bcrypt/bcrypt.h>

#include <stdexcept>

namespace {

constexpr int kBcryptWorkFactor = 10;

}  // namespace

std::string PasswordHasher::hash(const std::string& password) {
    char salt[BCRYPT_HASHSIZE];
    char out[BCRYPT_HASHSIZE];
    if (bcrypt_gensalt(kBcryptWorkFactor, salt) != 0) {
        throw std::runtime_error("bcrypt_gensalt failed");
    }
    if (bcrypt_hashpw(password.c_str(), salt, out) != 0) {
        throw std::runtime_error("bcrypt_hashpw failed");
    }
    return std::string(out);
}

bool PasswordHasher::verify(const std::string& password, const std::string& storedHash) {
    return bcrypt_checkpw(password.c_str(), storedHash.c_str()) == 0;
}
