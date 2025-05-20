#pragma once
#include <crow.h>
#include "../database/expenses_dao.h"
#include <nlohmann/json.hpp>

#include "middleware/auth_middleware.h"

using json = nlohmann::json;

class ExpensesAPI {
private:
    ExpensesDAO& expenses_dao;

public:
    explicit ExpensesAPI(ExpensesDAO& dao); 
    void registerRoutes(crow::App<AuthMiddleware>& app);

private:
    crow::response handleAddExpense(const crow::request& req, uint32_t user_id);
    crow::response handleGetExpenses(const crow::request& req, uint32_t user_id);
    crow::response handleDeleteExpense(int id, uint32_t user_id);
};
