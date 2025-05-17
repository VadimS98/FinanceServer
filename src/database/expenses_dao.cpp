#include <sqlite3.h>
#include <iostream>

#include "expenses_dao.h"
#include "utils/convert_utils.h"

ExpensesDAO::ExpensesDAO(DBManager& manager) : db_manager(manager) {}

bool ExpensesDAO::createTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS expenses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "amount REAL NOT NULL, "
        "currency TEXT NOT NULL, "
        "category TEXT NOT NULL, "
        "comment TEXT, "
        "timestamp INTEGER NOT NULL, "
        "user_id INTEGER NOT NULL);"; // Добавлен user_id

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_manager.getDB(), sql, nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

int ExpensesDAO::addExpense(const Expense& expense) {
    const char* sql =
        "INSERT INTO expenses (amount, currency, category, comment, timestamp, user_id) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_manager.getDB(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_manager.getDB()) << std::endl;
        return -1;
    }

    // Привязка параметров
    sqlite3_bind_double(stmt, 1, expense.amount);
    sqlite3_bind_text(stmt, 2, expense.currency.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, expense.category.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, expense.comment.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 5, expense.timestamp);
    sqlite3_bind_int(stmt, 6, expense.user_id); // user_id

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db_manager.getDB()) << std::endl;
        return -1;
    }

    return sqlite3_last_insert_rowid(db_manager.getDB());
}

std::vector<Expense> ExpensesDAO::getExpenses(uint32_t user_id, time_t from_date, time_t to_date) {
    std::vector<Expense> expenses;

    std::string sql =
        "SELECT id, amount, currency, category, comment, timestamp "
        "FROM expenses "
        "WHERE user_id = ? ";

    // Добавляем фильтры по дате, если они заданы
    if (from_date > 0) sql += "AND timestamp >= ? ";
    if (to_date > 0) sql += "AND timestamp <= ? ";

    sql += "ORDER BY timestamp DESC;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_manager.getDB(), sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_manager.getDB()) << std::endl;
        return expenses;
    }

    // Привязка параметров
    int param_index = 1;
    sqlite3_bind_int(stmt, param_index++, user_id);

    if (from_date > 0) sqlite3_bind_int64(stmt, param_index++, from_date);
    if (to_date > 0) sqlite3_bind_int64(stmt, param_index++, to_date);

    // Чтение результатов
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        Expense expense;
        expense.id = sqlite3_column_int(stmt, 0);
        expense.amount = sqlite3_column_double(stmt, 1);
        expense.currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        expense.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        expense.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        expense.timestamp = sqlite3_column_int64(stmt, 5);
        expense.user_id = user_id; // user_id берётся из запроса

        expenses.push_back(expense);
    }

    sqlite3_finalize(stmt);
    return expenses;
}

bool ExpensesDAO::deleteExpense(uint32_t id, uint32_t user_id) {
    const char* sql = "DELETE FROM expenses WHERE id = ? AND user_id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_manager.getDB(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_manager.getDB()) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, user_id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

std::optional<Expense> ExpensesDAO::getExpenseById(uint32_t id, uint32_t user_id) {
    const char* sql =
        "SELECT id, amount, currency, category, comment, timestamp "
        "FROM expenses WHERE id = ? AND user_id = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_manager.getDB(), sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db_manager.getDB()) << std::endl;
        return std::nullopt;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_int(stmt, 2, user_id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Expense expense;
        expense.id = sqlite3_column_int(stmt, 0);
        expense.amount = sqlite3_column_double(stmt, 1);
        expense.currency = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        expense.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        expense.comment = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        expense.timestamp = sqlite3_column_int64(stmt, 5);
        expense.user_id = user_id;

        sqlite3_finalize(stmt);
        return expense;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}