#include "Parser.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
const Token& Parser::peek() const { return tokens[current]; }
const Token& Parser::previous() const { return tokens[current - 1]; }
const Token& Parser::advance() { if (!isAtEnd()) ++current; return previous(); }
bool Parser::check(TokenType type) const { return peek().type == type; }
bool Parser::match(TokenType type) { if (!check(type)) return false; advance(); return true; }

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Line " + std::to_string(peek().line) + ": " + message);
}

void Parser::run() {
    while (!isAtEnd()) declaration();
}

void Parser::declaration() {
    if (match(TokenType::Purno) || match(TokenType::Doshomik) || match(TokenType::Lekha)) {
        // The declared type is currently used for syntax and basic value validation.
        TokenType declaredType = previous().type;
        const Token& name = consume(TokenType::Identifier, "Expected a variable name after type.");
        consume(TokenType::Assign, "Expected '=' after variable name.");
        Value value = expression();
        if (declaredType == TokenType::Lekha && !std::holds_alternative<std::string>(value))
            throw std::runtime_error("Line " + std::to_string(name.line) + ": 'lekha' requires a string value.");
        if (declaredType != TokenType::Lekha && !isNumber(value))
            throw std::runtime_error("Line " + std::to_string(name.line) + ": numeric variable requires a numeric value.");
        if (declaredType == TokenType::Purno && isNumber(value) && std::floor(asNumber(value)) != asNumber(value))
            throw std::runtime_error("Line " + std::to_string(name.line) + ": 'purno' requires an integer value.");
        symbols.define(name.lexeme, value);
        consume(TokenType::Semicolon, "Expected ';' after variable declaration.");
    } else {
        statement();
    }
}

void Parser::statement() {
    if (!check(TokenType::Identifier)) {
        throw std::runtime_error("Line " + std::to_string(peek().line) +
                                 ": expected a declaration, assignment, or function call.");
    }

    const Token name = advance();
    if (match(TokenType::Assign)) {
        Value value = expression();
        symbols.assign(name.lexeme, value);
        consume(TokenType::Semicolon, "Expected ';' after assignment.");
        return;
    }

    if (check(TokenType::LeftParen)) {
        --current; // Let expression() consume the function name and '('.
        printStatement();
        return;
    }

    throw std::runtime_error("Line " + std::to_string(peek().line) +
                             ": expected '=' or '(' after identifier '" + name.lexeme + "'.");
}

void Parser::printStatement() {
    Value value = expression();
    consume(TokenType::Semicolon, "Expected ';' after function call.");
    (void)value;
}

void Parser::expressionStatement() { (void)expression(); }

Value Parser::expression() { return equality(); }

Value Parser::equality() {
    Value left = comparison();
    while (match(TokenType::Equal) || match(TokenType::NotEqual)) {
        TokenType op = previous().type;
        Value right = comparison();
        bool equal = valueToString(left) == valueToString(right);
        left = (op == TokenType::Equal ? equal : !equal) ? 1.0 : 0.0;
    }
    return left;
}

Value Parser::comparison() {
    Value left = term();
    while (match(TokenType::Greater) || match(TokenType::Less) ||
           match(TokenType::GreaterEqual) || match(TokenType::LessEqual)) {
        TokenType op = previous().type;
        Value right = term();
        double a = asNumber(left), b = asNumber(right);
        bool result = false;
        if (op == TokenType::Greater) result = a > b;
        if (op == TokenType::Less) result = a < b;
        if (op == TokenType::GreaterEqual) result = a >= b;
        if (op == TokenType::LessEqual) result = a <= b;
        left = result ? 1.0 : 0.0;
    }
    return left;
}

Value Parser::term() {
    Value left = factor();
    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        TokenType op = previous().type;
        Value right = factor();
        double a = asNumber(left), b = asNumber(right);
        left = op == TokenType::Plus ? a + b : a - b;
    }
    return left;
}

Value Parser::factor() {
    Value left = unary();
    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        TokenType op = previous().type;
        Value right = unary();
        double a = asNumber(left), b = asNumber(right);
        if ((op == TokenType::Slash || op == TokenType::Percent) && b == 0)
            throw std::runtime_error("Division by zero.");
        if (op == TokenType::Star) left = a * b;
        else if (op == TokenType::Slash) left = a / b;
        else left = std::fmod(a, b);
    }
    return left;
}

Value Parser::unary() {
    if (match(TokenType::Minus)) return -asNumber(unary());
    if (match(TokenType::Bang)) return asNumber(unary()) == 0 ? 1.0 : 0.0;
    return primary();
}

Value Parser::primary() {
    if (match(TokenType::Number)) return std::stod(previous().lexeme);
    if (match(TokenType::String)) return previous().lexeme.substr(1, previous().lexeme.size() - 2);
    if (match(TokenType::Identifier)) {
        std::string name = previous().lexeme;
        if (match(TokenType::LeftParen)) return functionCall(name);
        return symbols.get(name);
    }
    if (match(TokenType::LeftParen)) {
        Value value = expression();
        consume(TokenType::RightParen, "Expected ')' after expression.");
        return value;
    }
    throw std::runtime_error("Line " + std::to_string(peek().line) + ": expected an expression.");
}

void Parser::requireNumber(const Value& value, const std::string& functionName) const {
    if (!isNumber(value)) throw std::runtime_error(functionName + " expects numeric arguments.");
}

Value Parser::functionCall(const std::string& name) {
    std::vector<Value> args;
    if (!check(TokenType::RightParen)) {
        do { args.push_back(expression()); } while (match(TokenType::Comma));
    }
    consume(TokenType::RightParen, "Expected ')' after function arguments.");

    if (name == "Dekhao") {
        if (args.size() != 1) throw std::runtime_error("Dekhao expects exactly one argument.");
        std::cout << valueToString(args[0]) << '\n';
        return 0.0;
    }
    if (name == "Nao") {
        if (!args.empty()) throw std::runtime_error("Nao expects no arguments.");
        double input;
        std::cout << "Input: ";
        if (!(std::cin >> input)) throw std::runtime_error("Could not read numeric input.");
        return input;
    }
    if (name == "Borgomul" || name == "Porom") {
        if (args.size() != 1) throw std::runtime_error(name + " expects one argument.");
        requireNumber(args[0], name);
        double x = asNumber(args[0]);
        if (name == "Borgomul" && x < 0) throw std::runtime_error("Borgomul cannot process a negative number.");
        return name == "Borgomul" ? std::sqrt(x) : std::fabs(x);
    }
    if (name == "Gunfol") {
        if (args.size() != 1) throw std::runtime_error("Gunfol expects one argument.");
        requireNumber(args[0], name);
        int n = static_cast<int>(asNumber(args[0]));
        if (n < 0 || n > 170 || n != asNumber(args[0])) throw std::runtime_error("Gunfol expects an integer from 0 to 170.");
        double result = 1;
        for (int i = 2; i <= n; ++i) result *= i;
        return result;
    }
    if (name == "Baraw" || name == "Komaw") {
        if (args.size() != 1) throw std::runtime_error(name + " expects one argument.");
        requireNumber(args[0], name);
        return asNumber(args[0]) + (name == "Baraw" ? 1 : -1);
    }
    if (name == "Elomelo") {
        if (!args.empty()) throw std::runtime_error("Elomelo expects no arguments in this version.");
        return 4.0; // deterministic starter value; random support will be added later.
    }

    if (args.size() != 2) throw std::runtime_error(name + " expects two arguments.");
    requireNumber(args[0], name); requireNumber(args[1], name);
    double a = asNumber(args[0]), b = asNumber(args[1]);
    if (name == "Jog") return a + b;
    if (name == "Biyog") return a - b;
    if (name == "Gun") return a * b;
    if (name == "Vag") {
        if (b == 0) throw std::runtime_error("Division by zero in Vag().");
        return a / b;
    }
    if (name == "Vagsesh") {
        if (b == 0) throw std::runtime_error("Division by zero in Vagsesh().");
        return std::fmod(a, b);
    }
    if (name == "Ghat") return std::pow(a, b);
    if (name == "Boro") return a > b ? a : b;
    if (name == "Choto") return a < b ? a : b;
    if (name == "Gor") return (a + b) / 2.0;
    if (name == "BoroJodi") return a > b ? 1.0 : 0.0;
    if (name == "ChotoJodi") return a < b ? 1.0 : 0.0;
    if (name == "Shoman") return a == b ? 1.0 : 0.0;
    if (name == "Oshoman") return a != b ? 1.0 : 0.0;
    throw std::runtime_error("Unknown function '" + name + "'.");
}

void Parser::printSymbols() const {
    std::cout << "\n--- SYMBOL TABLE ---\n";
    std::cout << "(Symbol table display will be enabled in the next milestone.)\n";
}
