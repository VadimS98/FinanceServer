#pragma once
#include "currency_rates_dao.h"
#include "db_manager.h"

class CurrencyRatesDAOSQLite : public CurrencyRatesDAO {
public:
    CurrencyRatesDAOSQLite(DBManager& db_manager);

    void updateRate(const CurrencyRate& rate) override;
    std::optional<CurrencyRate> getRate(const std::string& base, const std::string& target, time_t date) override;
    std::vector<CurrencyRate> getRatesHistory(const std::string& base, const std::string& target, time_t from, time_t to) override;

private:
    DBManager& db_manager_;
};
