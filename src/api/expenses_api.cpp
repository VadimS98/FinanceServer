#include "expenses_api.h"
#include <ctime>
#include <sstream>

#ifdef DELETE
#undef DELETE
#endif

ExpensesAPI::ExpensesAPI(ExpensesDAO& dao) : expenses_dao(dao) {}

void ExpensesAPI::registerRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/expenses")
        .methods(crow::HTTPMethod::POST)
        ([this](const crow::request& req) {
        return handleAddExpense(req);
            });

    CROW_ROUTE(app, "/expenses")
        .methods(crow::HTTPMethod::GET)
        ([this](const crow::request& req) {
        return handleGetExpenses(req);
            });

    CROW_ROUTE(app, "/expenses/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([this](int id) {
        return handleDeleteExpense(id);
            });
}

crow::response ExpensesAPI::handleAddExpense(const crow::request& req) {
    try {
        auto j = json::parse(req.body);

        Expense expense;
        expense.amount = j["amount"].get<double>();
        expense.currency = j["currency"].get<std::string>();
        expense.category = j["category"].get<std::string>();
        expense.comment = j.value("comment", "");
        expense.timestamp = j.value("timestamp", std::time(nullptr));
        expense.user_id = 1; // Временная заглушка

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

crow::response ExpensesAPI::handleGetExpenses(const crow::request& req) {
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

        uint32_t user_id = 1; // Временная заглушка

        // Логирование параметров для отладки
        std::cout << "Request params - user_id: " << user_id
            << ", from: " << from
            << ", to: " << to << std::endl;

        auto expenses = expenses_dao.getExpenses(user_id, from, to);

        json response = json::array();
        for (const auto& e : expenses) {
            // Проверка валидности данных
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

crow::response ExpensesAPI::handleDeleteExpense(int id) {
    try {
        uint32_t user_id = 1; // Временная заглушка
        if (!expenses_dao.deleteExpense(id, user_id)) {
            return crow::response(404, json{ {"error", "Expense not found"} }.dump());
        }
        return crow::response(200, json{ {"status", "deleted"}, {"id", id} }.dump());

    }
    catch (const std::exception& e) {
        return crow::response(500, json{ {"error", e.what()} }.dump());
    }
}
