#include "SymbolTable.h"
#include <stdexcept>

void SymbolTable::define(const std::string& name, const Value& value) {
    if (contains(name)) throw std::runtime_error("Variable '" + name + "' is already declared.");
    symbols[name] = value;
}
void SymbolTable::assign(const std::string& name, const Value& value) {
    auto it = symbols.find(name);
    if (it == symbols.end()) throw std::runtime_error("Undeclared variable '" + name + "'.");
    it->second = value;
}
Value SymbolTable::get(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) throw std::runtime_error("Undeclared variable '" + name + "'.");
    return it->second;
}
bool SymbolTable::contains(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}
