#pragma once
#include <crow.h>

class ReceiptsAPI {
public:
    void registerRoutes(crow::SimpleApp& app);
    // В будущем: POST /receipts, GET /receipts
};
