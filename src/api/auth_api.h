#pragma once
#include <crow.h>
#include "../database/users_dao.h"
#include <jwt-cpp/jwt.h>
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>

class AuthAPI {
public:
    AuthAPI(UsersDAO& usersDao,
        const std::string& jwtSecret,
        const std::string& telegramBotToken);

    void registerRoutes(crow::SimpleApp& app);

private:
    UsersDAO& usersDao_;
    std::string jwtSecret_;
    std::string telegramBotToken_;

    std::string generateJWT(uint32_t user_id) const;
    std::optional<uint32_t> validateJWT(const std::string& token) const;
    bool verifyTelegramAuth(const crow::json::rvalue& data) const;
    static std::string hashPassword(const std::string& password);
};
