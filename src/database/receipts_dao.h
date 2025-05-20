#pragma once
#include <vector>
#include "../models/receipt.h"

class ReceiptsDAO {
public:
    virtual ~ReceiptsDAO() = default;
    virtual void addReceipt(const Receipt& receipt) = 0;
    virtual std::vector<Receipt> getReceipts(uint32_t user_id) = 0;
};
