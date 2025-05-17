#pragma once
#include "db_manager.h"
#include "../models/expense.h"
#include <vector>
#include <optional>

class ExpensesDAO {
private:
    DBManager& db_manager; // Ссылка на менеджер БД

public:
    ExpensesDAO(DBManager& manager); // Конструктор

    // Создание таблицы expenses (если не существует)
    bool createTable();

    // Добавление расхода (возвращает ID новой записи или -1 при ошибке)
    int addExpense(const Expense& expense);

    // Получение списка расходов для конкретного пользователя (с фильтрами по дате)
    std::vector<Expense> getExpenses(uint32_t user_id, time_t from_date = 0, time_t to_date = 0);

    // Удаление расхода по ID (только для указанного пользователя)
    bool deleteExpense(uint32_t id, uint32_t user_id);

    // Получение расхода по ID (только для указанного пользователя)
    std::optional<Expense> getExpenseById(uint32_t id, uint32_t user_id);
};
