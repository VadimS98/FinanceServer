#include "auth_api.h"
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std::chrono;

AuthAPI::AuthAPI(UsersDAO& usersDao,
    const std::string& jwtSecret,
    const std::string& telegramBotToken)
    : usersDao_(usersDao),
    jwtSecret_(jwtSecret),
    telegramBotToken_(telegramBotToken) {
}

// Генерация JWT токена
std::string AuthAPI::generateJWT(uint32_t user_id) const {
    auto token = jwt::create()
        .set_issuer("finance-server")
        .set_type("JWS")
        .set_payload_claim("user_id", jwt::claim(std::to_string(user_id)))
        .set_expires_at(system_clock::now() + 24h)
        .sign(jwt::algorithm::hs256{ jwtSecret_ });

    return token;
}

std::optional<uint32_t> AuthAPI::validateJWT(const std::string& token) const {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{ jwtSecret_ })
            .with_issuer("finapp");
        verifier.verify(decoded);
        auto user_id_claim = decoded.get_payload_claim("user_id").as_string();
        return static_cast<uint32_t>(std::stoul(user_id_claim));
    }
    catch (...) {
        return std::nullopt;
    }
}

// Проверка подписи Telegram
bool AuthAPI::verifyTelegramAuth(const crow::json::rvalue& data) const {
    // 1. Сбор данных
    std::vector<std::pair<std::string, std::string>> fields;
    for (const auto& item : data) {
        if (item.key() != "hash") {
            fields.emplace_back(item.key(), item.s());
        }
    }

    // 2. Сортировка по ключу
    std::sort(fields.begin(), fields.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // 3. Формирование строки данных
    std::ostringstream dataCheckString;
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) dataCheckString << '\n';
        dataCheckString << fields[i].first << "=" << fields[i].second;
    }

    // 4. Вычисление секретного ключа (SHA256 от токена бота)
    unsigned char secretKey[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char*>(telegramBotToken_.data()),
        telegramBotToken_.size(),
        secretKey
    );

    // 5. Вычисление HMAC-SHA256
    unsigned char hmacResult[EVP_MAX_MD_SIZE];
    unsigned int hmacLen;
    HMAC(
        EVP_sha256(),
        secretKey,
        SHA256_DIGEST_LENGTH,
        reinterpret_cast<const unsigned char*>(dataCheckString.str().data()),
        dataCheckString.str().size(),
        hmacResult,
        &hmacLen
    );

    // 6. Конвертация в hex-строку
    std::ostringstream oss;
    for (unsigned int i = 0; i < hmacLen; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(hmacResult[i]);
    }

    return (oss.str() == data["hash"].s());
}

// Хэширование пароля (упрощённый пример)
std::string AuthAPI::hashPassword(const std::string& password) {
    // В реальном проекте используйте bcrypt или Argon2
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char*>(password.data()),
        password.size(),
        hash
    );

    std::ostringstream oss;
    for (unsigned char i : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(i);
    }
    return oss.str();
}

void AuthAPI::registerRoutes(crow::SimpleApp& app) {
    // Регистрация по email/паролю
    CROW_ROUTE(app, "/auth/register").methods("POST"_method)
        ([this](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);
            if (!body || !body.has("email") || !body.has("password")) {
                crow::json::wvalue resp;
                resp["error"] = "Invalid request";
                return crow::response(400, resp);
            }

            const std::string email = body["email"].s();
            const std::string password = body["password"].s();

            if (usersDao_.getUserByEmail(email)) {
                crow::json::wvalue resp;
                resp["error"] = "Email already exists";
                return crow::response(409, resp);
            }

            User newUser{
                0,
                email,
                hashPassword(password),
                static_cast<time_t>(system_clock::to_time_t(system_clock::now())),
                0
            };

            if (!usersDao_.createUser(newUser)) {
                crow::json::wvalue resp;
                resp["error"] = "Database error";
                return crow::response(500, resp);
            }

            crow::json::wvalue resp;
            resp["status"] = "User created";
            return crow::response(201, resp);

        }
        catch (const std::exception& e) {
            crow::json::wvalue resp;
            resp["error"] = e.what();
            return crow::response(500, resp);
        }
            });

    // Логин по email/паролю
    CROW_ROUTE(app, "/auth/login").methods("POST"_method)
        ([this](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("email") || !body.has("password")) {
            crow::json::wvalue resp;
            resp["error"] = "Invalid request";
            return crow::response(400, resp);
        }

        const std::string email = body["email"].s();
        const std::string password = body["password"].s();

        auto userOpt = usersDao_.getUserByEmail(email);
        if (!userOpt || userOpt->password_hash != hashPassword(password)) {
            crow::json::wvalue resp;
            resp["error"] = "Invalid credentials";
            return crow::response(401, resp);
        }

        crow::json::wvalue response;
        response["token"] = generateJWT(userOpt->id);
        return crow::response(response);
            });

    // Аутентификация через Telegram
    CROW_ROUTE(app, "/auth/telegram").methods("POST"_method)
        ([this](const crow::request& req) {
        try {
            auto body = crow::json::load(req.body);
            if (!body || !body.has("id") || !body.has("hash")) {
                crow::json::wvalue resp;
                resp["error"] = "Invalid request";
                return crow::response(400, resp);
            }

            // Проверка подписи
            if (!verifyTelegramAuth(body)) {
                crow::json::wvalue resp;
                resp["error"] = "Invalid Telegram hash";
                return crow::response(401, resp);
            }

            // Проверка времени авторизации (не старше 1 дня)
            const time_t authTime = body["auth_date"].i();
            if (time(nullptr) - authTime > 86400) {
                crow::json::wvalue resp;
                resp["error"] = "Expired auth data";
                return crow::response(401, resp);
            }

            // Поиск/создание пользователя
            const uint64_t tgId = body["id"].u();
            auto userOpt = usersDao_.getUserByTelegramId(tgId);

            if (!userOpt) {
                std::string username = body.has("username") ? body["username"].s() : std::string{};

                if (!usersDao_.createUserWithTelegram(tgId, username, authTime)) {
                    crow::json::wvalue resp;
                    resp["error"] = "User creation failed";
                    return crow::response(500, resp);
                }
                userOpt = usersDao_.getUserByTelegramId(tgId);
            }

            // Генерация токена
            crow::json::wvalue response;
            response["token"] = generateJWT(userOpt->id);
            return crow::response(response);

        }
        catch (const std::exception& e) {
            crow::json::wvalue resp;
            resp["error"] = e.what();
            return crow::response(500, resp);
        }
            });
}
