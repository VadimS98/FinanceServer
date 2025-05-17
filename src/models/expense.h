// Структура расходов
#pragma once
#include <string>
#include <ctime>

struct Expense {
	uint32_t id = 0;
	double amount = 0.0;
	std::string currency{};
	std::string category{};
	std::string comment{};
	time_t timestamp = 0;
	uint32_t user_id = 0; 
}; 
