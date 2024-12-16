#pragma once

#include <queue>

#include <Arduino.h>

#include "Constants.h"
#include "GlobalFunctions.h"

class Lexer {
public:
  enum class TokenType {
    // Keywords
    Let,
    Const,
    If,
    Else,
    While,
    Break,

    // Literal Types
    Identifier,
    Number,

    Equals,
    ArithmeticOperator,  // +, -, *, /, %
    RelationalOperator,  // <, <=, >, >=, ==, !=
    LogicalOperator,     // not, and, or

    OpenParen,   // (
    CloseParen,  // )
    OpenBrace,   // {
    CloseBrace,  // }

    Semicolon,
    Comma,

    // End of file
    EndOfFile,
  };

  typedef struct Token {
    char* value;
    TokenType type;

    // Default constructor
    Token()
      : value(nullptr), type(TokenType::Identifier) {}

    // Parameterized constructor
    Token(char* _value, TokenType _type)
      : value(_value), type(_type) {}

    // Copy constructor
    Token(const Token& other)
      : type(other.type) {
      if (other.value) {
        value = new char[strlen(other.value) + 1];
        strcpy(value, other.value);
      } else {
        value = nullptr;
      }
    }

    // Move constructor
    Token(Token&& other) noexcept
      : value(other.value), type(other.type) {
      other.value = nullptr;  // Transfer ownership
    }

    ~Token() {
      if (value != nullptr) {
        delete[] value;
      }
    }
  } Token;

  std::queue<Token> tokenize(char* code, size_t len);
private:
  std::queue<Token> tokens;

  char keywordVal[keywordCount][6] = { "let", "const", "if", "else", "while", "break", "and", "or" };

  const Token keywords[keywordCount]{
    Token(keywordVal[0], TokenType::Let),
    Token(keywordVal[1], TokenType::Const),
    Token(keywordVal[2], TokenType::If),
    Token(keywordVal[3], TokenType::Else),
    Token(keywordVal[4], TokenType::While),
    Token(keywordVal[5], TokenType::Break),
    Token(keywordVal[6], TokenType::LogicalOperator),
    Token(keywordVal[7], TokenType::LogicalOperator),
  };

  void addToken(const char* src, size_t srcLen, TokenType tokenType);

  TokenType getIdentTokenType(const char* ident, size_t len);
};