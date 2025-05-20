#pragma once
#include <string>
#include <optional>
#include "../models/user.h"
#include "db_manager.h"

class UsersDAO {
public:
    virtual ~UsersDAO() = default;

    virtual bool createUser(const User& user) = 0;
    virtual std::optional<User> getUserByEmail(const std::string& email) = 0;
    virtual std::optional<User> getUserById(uint32_t id) = 0;
    virtual bool checkPassword(const std::string& email, const std::string& password) = 0;

    virtual std::optional<User> getUserByTelegramId(uint64_t telegram_id) = 0;
    virtual bool createUserWithTelegram(uint64_t telegram_id, const std::string& username, time_t create_date) = 0;
};

// Реализация для SQLite
class UsersDAOSQLite : public UsersDAO {
public:
    explicit UsersDAOSQLite(DBManager& dbManager);

    bool createUser(const User& user) override;
    std::optional<User> getUserByEmail(const std::string& email) override;
    std::optional<User> getUserById(uint32_t id) override;
    bool checkPassword(const std::string& email, const std::string& password) override;
    std::optional<User> getUserByTelegramId(uint64_t telegram_id) override;
    bool createUserWithTelegram(uint64_t telegram_id, const std::string& username, time_t create_date) override;

private:
    DBManager& dbManager_;
};
