#pragma once
#include <string>

enum class TokenType {
    EndOfFile, Identifier, Number, String,
    Purno, Doshomik, Lekha,
    Jodi, Nahole,
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Semicolon,
    Assign, Plus, Minus, Star, Slash, Percent,
    Equal, NotEqual, Greater, Less, GreaterEqual, LessEqual,
    Bang
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
