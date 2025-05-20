#include "auth_middleware.h"

AuthMiddleware::AuthMiddleware(const std::string& jwtSecret) : jwtSecret_(jwtSecret) {}

void AuthMiddleware::before_handle(crow::request& req, crow::response& res, context& ctx) {
    auto authHeader = req.get_header_value("Authorization");
    if (authHeader.substr(0, 7) == "Bearer ") {
        std::string token = authHeader.substr(7);
        try {
            auto decoded = jwt::decode(token);
            auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ jwtSecret_ })
                .with_issuer("finapp");
            verifier.verify(decoded);
            auto user_id_claim = decoded.get_payload_claim("user_id").as_string();
            ctx.user_id = static_cast<uint32_t>(std::stoul(user_id_claim));
        }
        catch (...) {
            res.code = 401;
            res.end("Invalid or expired token");
            return;
        }
    }
    else {
        res.code = 401;
        res.end("Missing Authorization header");
        return;
    }
}

void AuthMiddleware::after_handle(crow::request& req, crow::response& res, context& ctx) {}
