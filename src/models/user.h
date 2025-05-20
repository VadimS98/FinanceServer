#pragma once
#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

struct User {
    uint32_t id;
    std::string email;
    std::string password_hash;
    time_t create_date;
    uint64_t telegram_id = 0; // 0 если не задан
};

using json = nlohmann::json;

inline void to_json(json& j, const User& u) {
    j = json{
        {"id", u.id},
        {"email", u.email},
        {"password_hash", u.password_hash},
        {"create_date", u.create_date},
        {"telegram_id", u.telegram_id}
    };
}

inline void from_json(const json& j, User& u) {
    j.at("id").get_to(u.id);
    j.at("email").get_to(u.email);
    j.at("password_hash").get_to(u.password_hash);
    j.at("create_date").get_to(u.create_date);
    // telegram_id может быть необязательным (например, для старых пользователей)
    if (j.contains("telegram_id") && !j.at("telegram_id").is_null()) {
        j.at("telegram_id").get_to(u.telegram_id);
    }
    else {
        u.telegram_id = 0;
    }
}
