#pragma once
#include <crow.h>
#include <string>
#include <optional>
#include <jwt-cpp/jwt.h>

class AuthMiddleware {
public:
    struct context {
        std::optional<uint32_t> user_id;
    };

    AuthMiddleware() = default; // нужен для Crow
    AuthMiddleware(const std::string& jwtSecret);

    void before_handle(crow::request& req, crow::response& res, context& ctx);
    void after_handle(crow::request& req, crow::response& res, context& ctx);

private:
    std::string jwtSecret_;
};
