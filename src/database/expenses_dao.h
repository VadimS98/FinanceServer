#pragma once
#include "db_manager.h"
#include "../models/expense.h"
#include <vector>
#include <optional>

class ExpensesDAO {
private:
    DBManager& db_manager; // ������ �� �������� ��

public:
    ExpensesDAO(DBManager& manager); // �����������

    // �������� ������� expenses (���� �� ����������)
    bool createTable();

    // ���������� ������� (���������� ID ����� ������ ��� -1 ��� ������)
    int addExpense(const Expense& expense);

    // ��������� ������ �������� ��� ����������� ������������ (� ��������� �� ����)
    std::vector<Expense> getExpenses(uint32_t user_id, time_t from_date = 0, time_t to_date = 0);

    // �������� ������� �� ID (������ ��� ���������� ������������)
    bool deleteExpense(uint32_t id, uint32_t user_id);

    // ��������� ������� �� ID (������ ��� ���������� ������������)
    std::optional<Expense> getExpenseById(uint32_t id, uint32_t user_id);
};
