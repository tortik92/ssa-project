#pragma once

#include <Arduino.h>
#include <ctype.h>

#include "Constants.h"
#include "GlobalFunctions.h"

class Lexer {
public:
  enum class TokenType {
    // Keywords
    Let, Const,

    If,
    Else,
    While,
    For,
    // Literal Types
    Identifier,
    Number,
    String,

    Equals,
    ArithmeticOperator,

    OpenParen,
    CloseParen,

    Semicolon,
    // End of file
    EndOfFile,
  };

  typedef struct Token {
    char* value;
    TokenType type;

    Token() {
      value = nullptr;
      type = TokenType::Identifier;
    }

    Token(char* _value, TokenType _type)
      : value(_value), type(_type) {}

    ~Token() {
      delete[] value;
    }
  } Token;

  Token* tokenize(char* code, size_t len);
private:
  Token tokens[maxTokens];
  Token* currentToken;

  char keywordVal[keywordCount][6] = { "let", "const", "if", "else", "while", "for" };

  const Token keywords[keywordCount]{
    Token(keywordVal[0], TokenType::Let),
    Token(keywordVal[1], TokenType::Const),
    Token(keywordVal[2], TokenType::If),
    Token(keywordVal[3], TokenType::Else),
    Token(keywordVal[4], TokenType::While),
    Token(keywordVal[5], TokenType::For)
  };

  void addToken(Token* dest, const char* src, size_t srcLen, TokenType tokenType);

  TokenType getIdentTokenType(const char* ident, size_t len);
};