#pragma once
#include <crow.h>

class CurrencyRatesAPI {
public:
    void registerRoutes(crow::SimpleApp& app);
    // В будущем: обработчики GET /rates, POST /rates
};
