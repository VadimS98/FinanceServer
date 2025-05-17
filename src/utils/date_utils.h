#pragma once
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

std::string timestampToString(time_t timestamp) {
    struct tm timeinfo;
    localtime_s(&timeinfo, &timestamp); // Для Windows
    // Для Linux/Mac: localtime_r(&timestamp, &timeinfo);

    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}