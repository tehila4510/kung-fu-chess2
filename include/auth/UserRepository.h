#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include <string>

// SQLite-backed user store. All SQL stays in the .cpp — no business logic.
class UserRepository {
    void* db_ = nullptr;  // sqlite3*
    std::string path_;

    void ensureSchema();

public:
    explicit UserRepository(const std::string& dbPath);
    ~UserRepository();

    UserRepository(const UserRepository&) = delete;
    UserRepository& operator=(const UserRepository&) = delete;

    bool findByUsername(const std::string& username, std::string& outHash,
                        int& outRating) const;
    void insertUser(const std::string& username, const std::string& passwordHash,
                    int rating);
    int getRating(const std::string& username) const;
    void setRating(const std::string& username, int rating);
};

#endif
