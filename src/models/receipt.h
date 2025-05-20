#pragma once
#include <string>
#include <ctime>

struct Receipt {
    uint32_t id;
    uint32_t user_id;
    std::string file_path;
    std::string ocr_text;
    time_t uploaded_at;
};
