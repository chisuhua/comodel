#pragma once

#include <cstdio>
#include <string>
#include <vector>


template <typename ...Args>
inline std::string FormatString(const std::string& fmt, Args&& ...args) {
    size_t size = snprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...) + 1;
    std::vector<char> buf(size);
    snprintf(buf.data(), size, fmt.c_str(), std::forward<Args>(args)...);
    return std::string(buf.data(), buf.data()+size-1);
}

inline std::string GetFormatString() {
    return "";
}

inline std::string GetFormatString(const std::string& str) {
    return str;
}

template <typename ...Args>
inline std::string GetFormatString(const std::string& fmt, Args&& ...args) {
    return FormatString(fmt, std::forward<Args>(args)...);
}
