#include "users_dao.h"
#include "db_manager.h"
#include <sqlite3.h>
#include <optional>
#include <string>

// Конструктор, принимает ссылку на DBManager
UsersDAOSQLite::UsersDAOSQLite(DBManager& dbManager) : dbManager_(dbManager) {}

// Создание пользователя (email/пароль или Telegram)
bool UsersDAOSQLite::createUser(const User& user) {
    sqlite3* db = dbManager_.getDB();
    const char* sql = "INSERT INTO users (email, password_hash, create_date, telegram_id) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, user.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, static_cast<sqlite3_int64>(user.create_date));
    if (user.telegram_id != 0)
        sqlite3_bind_int64(stmt, 4, user.telegram_id);
    else
        sqlite3_bind_null(stmt, 4);

    bool result = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return result;
}

// Поиск пользователя по email
std::optional<User> UsersDAOSQLite::getUserByEmail(const std::string& email) {
    sqlite3* db = dbManager_.getDB();
    const char* sql = "SELECT id, email, password_hash, create_date, telegram_id FROM users WHERE email = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.create_date = static_cast<time_t>(sqlite3_column_int64(stmt, 3));
        u.telegram_id = sqlite3_column_type(stmt, 4) == SQLITE_NULL ? 0 : static_cast<uint64_t>(sqlite3_column_int64(stmt, 4));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

// Поиск пользователя по id
std::optional<User> UsersDAOSQLite::getUserById(uint32_t id) {
    sqlite3* db = dbManager_.getDB();
    const char* sql = "SELECT id, email, password_hash, create_date, telegram_id FROM users WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_int(stmt, 1, static_cast<int>(id));

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.create_date = static_cast<time_t>(sqlite3_column_int64(stmt, 3));
        u.telegram_id = sqlite3_column_type(stmt, 4) == SQLITE_NULL ? 0 : static_cast<uint64_t>(sqlite3_column_int64(stmt, 4));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

// Проверка пароля (email+пароль)
bool UsersDAOSQLite::checkPassword(const std::string& email, const std::string& password) {
    auto userOpt = getUserByEmail(email);
    if (!userOpt) return false;
    // Здесь должен быть хэш и сравнение пароля (например, bcrypt)
    // Для прототипа: сравниваем строку напрямую (НЕ использовать в продакшене)
    return userOpt->password_hash == password;
}

// Поиск пользователя по Telegram ID
std::optional<User> UsersDAOSQLite::getUserByTelegramId(uint64_t telegram_id) {
    sqlite3* db = dbManager_.getDB();
    const char* sql = "SELECT id, email, password_hash, create_date, telegram_id FROM users WHERE telegram_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return std::nullopt;

    sqlite3_bind_int64(stmt, 1, telegram_id);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        User u;
        u.id = static_cast<uint32_t>(sqlite3_column_int(stmt, 0));
        u.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        u.create_date = static_cast<time_t>(sqlite3_column_int64(stmt, 3));
        u.telegram_id = sqlite3_column_type(stmt, 4) == SQLITE_NULL ? 0 : static_cast<uint64_t>(sqlite3_column_int64(stmt, 4));
        result = u;
    }
    sqlite3_finalize(stmt);
    return result;
}

// Создание пользователя через Telegram
bool UsersDAOSQLite::createUserWithTelegram(uint64_t telegram_id, const std::string& username, time_t create_date) {
    sqlite3* db = dbManager_.getDB();
    const char* sql = "INSERT INTO users (email, password_hash, create_date, telegram_id) VALUES (?, '', ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    // Псевдо-email для Telegram-пользователя (можно заменить на username, если нужно)
    std::string pseudo_email = "tg_" + std::to_string(telegram_id) + "@telegram";
    sqlite3_bind_text(stmt, 1, pseudo_email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, static_cast<sqlite3_int64>(create_date));
    sqlite3_bind_int64(stmt, 3, telegram_id);

    bool result = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return result;
}
