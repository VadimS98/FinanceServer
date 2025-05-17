#pragma once
#include <string>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <locale>
    #include <codecvt>
#endif

inline std::string to_utf8(const std::wstring& wide_str) {
#ifdef _WIN32
    if (wide_str.empty()) return std::string();
    int size = WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return std::string();
    std::string utf8_str(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide_str.c_str(), -1, &utf8_str[0], size, nullptr, nullptr);
    return utf8_str;
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
    return conv.to_bytes(wide_str);
#endif
}
