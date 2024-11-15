#pragma once

#include <Arduino.h>
#include <ctype.h>

#include "Constants.h"
#include "GlobalFunctions.h"

class Lexer {
public:
  enum class TokenType {
    Var,

    If,
    Else,
    While,
    For,

    Identifier,
    Number,

    Equals,
    ArithmeticOperator,

    OpenParen,
    CloseParen,

    Undefined,
    EndOfFile,
  };

  typedef struct Token {
    char value[tokLen];
    TokenType type;

    Token() {
      strncpy(value, "\0", tokLen - 1);
      type = TokenType::Identifier;
    }

    Token(const char* val, TokenType t)
      : type(t) {
      strncpy(value, val, tokLen - 1);
      value[tokLen - 1] = '\0';
    }
  } Token;

  Token* tokenize(char* code, size_t len);
private:
  Token tokens[maxTokens];
  Token* currentToken;

  const Token keywords[keywordCount]{
    Token("var", TokenType::Var),
    Token("if", TokenType::If),
    Token("else", TokenType::Else),
    Token("while", TokenType::While),
    Token("for", TokenType::For)
  };

  void addToken(Token* dest, const char tokenValue[tokLen], size_t srcTokLen, TokenType tokenType);

  TokenType getIdentTokenType(const char* ident, size_t len);
};