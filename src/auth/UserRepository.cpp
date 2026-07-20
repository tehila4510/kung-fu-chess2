#include "auth/UserRepository.h"

#include "sqlite3.h"

#include <filesystem>
#include <stdexcept>

namespace {

void throwIfNotOk(int rc, sqlite3* db, const char* what) {
    if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
        const char* msg = db != nullptr ? sqlite3_errmsg(db) : "unknown";
        throw std::runtime_error(std::string(what) + ": " + msg);
    }
}

}  // namespace

UserRepository::UserRepository(const std::string& dbPath) : path_(dbPath) {
    const std::filesystem::path path(dbPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    sqlite3* raw = nullptr;
    const int rc = sqlite3_open(dbPath.c_str(), &raw);
    db_ = raw;
    if (rc != SQLITE_OK) {
        const std::string msg = raw != nullptr ? sqlite3_errmsg(raw) : "open failed";
        if (raw != nullptr) {
            sqlite3_close(raw);
        }
        db_ = nullptr;
        throw std::runtime_error("sqlite open failed: " + msg);
    }
    ensureSchema();
}

UserRepository::~UserRepository() {
    if (db_ != nullptr) {
        sqlite3_close(static_cast<sqlite3*>(db_));
        db_ = nullptr;
    }
}

void UserRepository::ensureSchema() {
    auto* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  username TEXT PRIMARY KEY NOT NULL,"
        "  password_hash TEXT NOT NULL,"
        "  rating INTEGER NOT NULL DEFAULT 1200"
        ");";
    char* err = nullptr;
    const int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string msg = err != nullptr ? err : "schema failed";
        sqlite3_free(err);
        throw std::runtime_error(msg);
    }
}

bool UserRepository::findByUsername(const std::string& username, std::string& outHash,
                                    int& outRating) const {
    auto* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT password_hash, rating FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throwIfNotOk(rc, db, "prepare findByUsername");

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        throwIfNotOk(rc, db, "bind findByUsername");
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* hashText = sqlite3_column_text(stmt, 0);
        outHash = hashText != nullptr ? reinterpret_cast<const char*>(hashText) : "";
        outRating = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        throwIfNotOk(rc, db, "step findByUsername");
    }
    return false;
}

void UserRepository::insertUser(const std::string& username,
                                const std::string& passwordHash, int rating) {
    auto* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "INSERT INTO users (username, password_hash, rating) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throwIfNotOk(rc, db, "prepare insertUser");

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, rating);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        throwIfNotOk(rc, db, "insertUser");
    }
}

int UserRepository::getRating(const std::string& username) const {
    std::string hash;
    int rating = 0;
    if (!findByUsername(username, hash, rating)) {
        throw std::invalid_argument("unknown user: " + username);
    }
    return rating;
}

void UserRepository::setRating(const std::string& username, int rating) {
    auto* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE users SET rating = ? WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    throwIfNotOk(rc, db, "prepare setRating");

    sqlite3_bind_int(stmt, 1, rating);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        throwIfNotOk(rc, db, "setRating");
    }
    if (sqlite3_changes(db) == 0) {
        throw std::invalid_argument("unknown user: " + username);
    }
}
