#pragma once
#include "Token.h"
#include <string>
#include <vector>

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    std::size_t start = 0;
    std::size_t current = 0;
    int line = 1;
    int column = 1;
    int tokenColumn = 1;

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    void add(TokenType type);
    void scanToken();
    void identifier();
    void number();
    void stringLiteral();
    void skipLineComment();
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);
};
