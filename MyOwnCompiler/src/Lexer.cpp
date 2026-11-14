#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : source(source) {}

bool Lexer::isAtEnd() const { return current >= source.size(); }

char Lexer::advance() {
    char c = source[current++];
    if (c == '\n') { ++line; column = 1; }
    else { ++column; }
    return c;
}

char Lexer::peek() const { return isAtEnd() ? '\0' : source[current]; }
char Lexer::peekNext() const { return current + 1 >= source.size() ? '\0' : source[current + 1]; }

bool Lexer::match(char expected) {
    if (isAtEnd() || source[current] != expected) return false;
    advance();
    return true;
}

void Lexer::add(TokenType type) {
    tokens.push_back({type, source.substr(start, current - start), line, tokenColumn});
}

bool Lexer::isAlpha(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}
bool Lexer::isDigit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }
bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

void Lexer::skipLineComment() {
    while (peek() != '\n' && !isAtEnd()) advance();
}

void Lexer::identifier() {
    while (isAlphaNumeric(peek())) advance();
    std::string text = source.substr(start, current - start);
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"purno", TokenType::Purno},
        {"doshomik", TokenType::Doshomik},
        {"lekha", TokenType::Lekha},
        {"jodi", TokenType::Jodi},
        {"nahole", TokenType::Nahole}
    };
    auto it = keywords.find(text);
    add(it == keywords.end() ? TokenType::Identifier : it->second);
}

void Lexer::number() {
    while (isDigit(peek())) advance();
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }
    add(TokenType::Number);
}

void Lexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') throw std::runtime_error("Line " + std::to_string(line) + ": unterminated string literal");
        advance();
    }
    if (isAtEnd()) throw std::runtime_error("Line " + std::to_string(line) + ": unterminated string literal");
    advance(); // closing quote
    add(TokenType::String);
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': add(TokenType::LeftParen); break;
        case ')': add(TokenType::RightParen); break;
        case '{': add(TokenType::LeftBrace); break;
        case '}': add(TokenType::RightBrace); break;
        case ',': add(TokenType::Comma); break;
        case ';': add(TokenType::Semicolon); break;
        case '+': add(TokenType::Plus); break;
        case '-': add(TokenType::Minus); break;
        case '*': add(TokenType::Star); break;
        case '%': add(TokenType::Percent); break;
        case '!': add(match('=') ? TokenType::NotEqual : TokenType::Bang); break;
        case '=': add(match('=') ? TokenType::Equal : TokenType::Assign); break;
        case '>': add(match('=') ? TokenType::GreaterEqual : TokenType::Greater); break;
        case '<': add(match('=') ? TokenType::LessEqual : TokenType::Less); break;
        case '/':
            if (match('/')) skipLineComment();
            else add(TokenType::Slash);
            break;
        case '"': stringLiteral(); break;
        case ' ': case '\r': case '\t': break;
        case '\n': break;
        default:
            if (isDigit(c)) number();
            else if (isAlpha(c)) identifier();
            else throw std::runtime_error("Line " + std::to_string(line) +
                ", column " + std::to_string(tokenColumn) + ": unexpected character '" + c + "'");
    }
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        tokenColumn = column;
        scanToken();
    }
    tokens.push_back({TokenType::EndOfFile, "", line, column});
    return tokens;
}
