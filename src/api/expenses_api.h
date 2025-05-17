#pragma once
#include <crow.h>
#include "../database/expenses_dao.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ExpensesAPI {
private:
    ExpensesDAO& expenses_dao;

public:
    explicit ExpensesAPI(ExpensesDAO& dao); 
    void registerRoutes(crow::SimpleApp& app);

private:
    crow::response handleAddExpense(const crow::request& req);
    crow::response handleGetExpenses(const crow::request& req);
    crow::response handleDeleteExpense(int id);
};
