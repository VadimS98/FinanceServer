#include "expenses_api.h"
#include <ctime>
#include <sstream>

#ifdef DELETE
#undef DELETE
#endif

ExpensesAPI::ExpensesAPI(ExpensesDAO& dao) : expenses_dao(dao) {}

void ExpensesAPI::registerRoutes(crow::App<AuthMiddleware>& app) {
    // Добавление расхода (user_id только из JWT)
    CROW_ROUTE(app, "/expenses")
        .methods(crow::HTTPMethod::POST)
        ([this, &app](const crow::request& req) {
        auto& ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.user_id) {
            return crow::response(401, "Unauthorized");
        }
        return handleAddExpense(req, *ctx.user_id);
            });

    // Получение расходов пользователя (user_id только из JWT)
    CROW_ROUTE(app, "/expenses")
        .methods(crow::HTTPMethod::GET)
        ([this, &app](const crow::request& req) {
        auto& ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.user_id) {
            return crow::response(401, "Unauthorized");
        }
        return handleGetExpenses(req, *ctx.user_id);
            });

    // Удаление расхода по id (user_id только из JWT)
    CROW_ROUTE(app, "/expenses/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([this, &app](const crow::request& req, int id) {
        auto& ctx = app.get_context<AuthMiddleware>(req);
        if (!ctx.user_id) {
            return crow::response(401, "Unauthorized");
        }
        return handleDeleteExpense(id, *ctx.user_id);
            });
}


// Добавление расхода — user_id только из JWT!
crow::response ExpensesAPI::handleAddExpense(const crow::request& req, uint32_t user_id) {
    try {
        auto j = json::parse(req.body);

        Expense expense;
        expense.amount = j["amount"].get<double>();
        expense.currency = j["currency"].get<std::string>();
        expense.category = j["category"].get<std::string>();
        expense.comment = j.value("comment", "");
        expense.timestamp = j.value("timestamp", std::time(nullptr));
        expense.user_id = user_id; // Только из JWT!

        int id = expenses_dao.addExpense(expense);
        if (id < 0) return crow::response(500, "Internal server error");

        json response = {
            {"id", id},
            {"status", "success"}
        };
        return crow::response(201, response.dump());

    }
    catch (const std::exception& e) {
        return crow::response(400, json{ {"error", e.what()} }.dump());
    }
}

// Получение расходов пользователя — user_id только из JWT!
crow::response ExpensesAPI::handleGetExpenses(const crow::request& req, uint32_t user_id) {
    try {
        time_t from = 0, to = 0;

        // Обработка параметра 'from'
        if (req.url_params.get("from") != nullptr) {
            try {
                from = std::stoll(req.url_params.get("from"));
            }
            catch (const std::exception& e) {
                return crow::response(400, json{ {"error", "Invalid 'from' parameter"} }.dump());
            }
        }

        // Обработка параметра 'to'
        if (req.url_params.get("to") != nullptr) {
            try {
                to = std::stoll(req.url_params.get("to"));
            }
            catch (const std::exception& e) {
                return crow::response(400, json{ {"error", "Invalid 'to' parameter"} }.dump());
            }
        }

        // user_id только из JWT!
        auto expenses = expenses_dao.getExpenses(user_id, from, to);

        json response = json::array();
        for (const auto& e : expenses) {
            if (std::isnan(e.amount) || std::isinf(e.amount)) {
                std::cerr << "Invalid amount value in expense ID: " << e.id << std::endl;
                continue;
            }

            response.push_back({
                {"id", e.id},
                {"amount", e.amount},
                {"currency", e.currency},
                {"category", e.category},
                {"comment", e.comment},
                {"timestamp", e.timestamp}
                });
        }

        return crow::response(200, response.dump());

    }
    catch (const std::exception& e) {
        std::cerr << "Exception in handleGetExpenses: " << e.what() << std::endl;
        return crow::response(500, json{ {"error", "Internal server error GET"} }.dump());
    }
}

// Удаление расхода — user_id только из JWT!
crow::response ExpensesAPI::handleDeleteExpense(int id, uint32_t user_id) {
    try {
        if (!expenses_dao.deleteExpense(id, user_id)) {
            return crow::response(404, json{ {"error", "Expense not found"} }.dump());
        }
        return crow::response(200, json{ {"status", "deleted"}, {"id", id} }.dump());

    }
    catch (const std::exception& e) {
        return crow::response(500, json{ {"error", e.what()} }.dump());
    }
}
