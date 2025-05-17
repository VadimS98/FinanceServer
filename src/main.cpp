#include <crow.h>
#include <sqlite3.h>

#include "database/db_manager.h"
#include "database/expenses_dao.h"
#include "api/expenses_api.h"

#include "utils/date_utils.h"


// Заполнение тестовыми расходами
void fillTestExpenses(ExpensesDAO& dao, uint32_t user_id) {
    Expense e1{ 0, 500.0, u8"RUB", u8"АПАППАВвававапр", "dinner", time(nullptr), user_id };
    Expense e2{ 0, 1200.0, "USD", "transport", "taxi", time(nullptr), user_id };
    Expense e3{ 0, 300.0, "EUR", "entartainment", "cinema", time(nullptr), user_id };

    dao.addExpense(e1);
    dao.addExpense(e2);
    dao.addExpense(e3);
}

// Вывод расходов в консоль
void printAllExpenses(ExpensesDAO& dao, uint32_t user_id) {
    auto expenses = dao.getExpenses(user_id);
    for (const auto& e : expenses) {
        std::cout << "ID: " << e.id
            << ", Amount: " << e.amount
            << ", Currency: " << e.currency
            << ", Category: " << e.category
            << ", Comment: " << e.comment
            << ", Timestamp: " << timestampToString(e.timestamp)
            << ", UserID: " << e.user_id
            << std::endl;
    }
}

int main() {

    DBManager db_manager("expenses.db");
    if (!db_manager.initialize()) {
        std::cerr << "Failed to initialize database!" << std::endl;
        return 1;
    }

    // Создание DAO и таблицы
    ExpensesDAO expenses_dao(db_manager);
    if (!expenses_dao.createTable()) {
        std::cerr << "Failed to create expenses table!" << std::endl;
        return 1;
    }

    fillTestExpenses(expenses_dao, 1);

    printAllExpenses(expenses_dao, 0);
    std::cout << std::endl;
    printAllExpenses(expenses_dao, 1);
    std::cout << std::endl;
    printAllExpenses(expenses_dao, 2);
    std::cout << std::endl;


    crow::SimpleApp app;

    // Регистрация API
    ExpensesAPI expenses_api(expenses_dao);
    expenses_api.registerRoutes(app);

    app.port(8080).multithreaded().run();

    return 0;
}
