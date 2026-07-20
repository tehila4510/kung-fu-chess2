#ifndef AUTH_RESULT_H
#define AUTH_RESULT_H

#include <string>

struct AuthResult {
    bool ok = false;
    std::string reason;
    int rating = 0;
};

#endif
