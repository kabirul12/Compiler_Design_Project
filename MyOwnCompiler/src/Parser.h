#pragma once
#include "Token.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    void run();
    void printSymbols() const;

private:
    const std::vector<Token>& tokens;
    std::size_t current = 0;
    SymbolTable symbols;

    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& message);
    void declaration();
    void statement();
    void variableDeclaration();
    void printStatement();
    void expressionStatement();
    Value expression();
    Value equality();
    Value comparison();
    Value term();
    Value factor();
    Value unary();
    Value primary();
    Value functionCall(const std::string& name);
    void requireNumber(const Value& value, const std::string& functionName) const;
};
