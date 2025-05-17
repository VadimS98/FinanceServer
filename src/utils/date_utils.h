#pragma once
#include <ctime>
#include <string>
#include <iomanip>
#include <sstream>

inline std::string timestampToString(time_t timestamp) {
    std::tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &timestamp); // Windows
#else
    localtime_r(&timestamp, &timeinfo); // Linux/macOS
#endif

    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
