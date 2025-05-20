#pragma once
#include <string>
#include <vector>
#include <optional>
#include <ctime>

struct CurrencyRate {
    std::string base_currency;
    std::string target_currency;
    double rate;
    time_t timestamp;
};

class CurrencyRatesDAO {
public:
    virtual ~CurrencyRatesDAO() = default;
    virtual void updateRate(const CurrencyRate& rate) = 0;
    virtual std::optional<CurrencyRate> getRate(const std::string& base, const std::string& target, time_t date) = 0;
    virtual std::vector<CurrencyRate> getRatesHistory(const std::string& base, const std::string& target, time_t from, time_t to) = 0;
};
