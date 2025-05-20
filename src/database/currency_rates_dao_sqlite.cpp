#include "currency_rates_dao_sqlite.h"

CurrencyRatesDAOSQLite::CurrencyRatesDAOSQLite(DBManager& db_manager)
    : db_manager_(db_manager) {
}

void CurrencyRatesDAOSQLite::updateRate(const CurrencyRate& /*rate*/) {
    // Заглушка
}

std::optional<CurrencyRate> CurrencyRatesDAOSQLite::getRate(const std::string&, const std::string&, time_t) {
    // Заглушка
    return std::nullopt;
}

std::vector<CurrencyRate> CurrencyRatesDAOSQLite::getRatesHistory(const std::string&, const std::string&, time_t, time_t) {
    // Заглушка
    return {};
}
