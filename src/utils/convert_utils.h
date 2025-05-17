#pragma once
#include <windows.h>
#include <string>

std::string to_utf8(const std::wstring& wide_str) {
    int size = WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, &utf8_str[0], size, nullptr, nullptr);
    return utf8_str;
}