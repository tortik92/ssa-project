#pragma once

#ifndef PIO_UNIT_TESTING
#include <Arduino.h>
#endif

#include <ctype.h>
#include "Constants.h"

class Lexer {
public:
    Lexer() {
        currentToken = &tokens[0];
    }

    enum class TokenType {
        Var,
        If, Else,
        While, For,
        Identifier,
        Equals,
        ArithmeticOperator,
        OpenParen, CloseParen,
        Number,
    };

    typedef struct Token {
        char* value;
        TokenType type;
    } Token;

    const Token keywords[5] {
        {"var", TokenType::Var},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"while", TokenType::While},
        {"for", TokenType::For}
    };

    Token* tokenize(char* code, size_t len);
private:
    Token tokens[maxTokens];
    Token* currentToken;

    void addToken(const char* c, size_t len, TokenType tokenType);

    TokenType getIdentTokenType(const char* ident);
    bool isSkippable(const char c);
};