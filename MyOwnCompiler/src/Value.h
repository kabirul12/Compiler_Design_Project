#pragma once
#include <string>
#include <variant>
#include <stdexcept>

using Value = std::variant<double, std::string>;

inline bool isNumber(const Value& value) {
    return std::holds_alternative<double>(value);
}
inline double asNumber(const Value& value) {
    if (!isNumber(value)) throw std::runtime_error("Expected a numeric value.");
    return std::get<double>(value);
}
inline std::string valueToString(const Value& value) {
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    double n = std::get<double>(value);
    std::string s = std::to_string(n);
    while (s.find('.') != std::string::npos && s.back() == '0') s.pop_back();
    if (!s.empty() && s.back() == '.') s.pop_back();
    return s;
}
