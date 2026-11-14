#pragma once
#include "Value.h"
#include <string>
#include <unordered_map>

class SymbolTable {
public:
    void define(const std::string& name, const Value& value);
    void assign(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    bool contains(const std::string& name) const;

private:
    std::unordered_map<std::string, Value> symbols;
};
