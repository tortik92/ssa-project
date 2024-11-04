#include "Lexer.hpp"


Lexer::Token* Lexer::tokenize(char* code, size_t len) {
  for (size_t i = 0; i < len; i++) {
    const char c[2] = {code[i], '\0'};
    switch (code[i]) {
      case '(':
        addToken(c, 2, TokenType::OpenParen);
        break;
      case ')':
        addToken(c, 2, TokenType::CloseParen);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        addToken(c, 2, TokenType::ArithmeticOperator);
        break;
      case '=':
        addToken(c, 2, TokenType::Equals);
        break;
      default:
        // handle multicharacter tokens

        if (isDigit(code[i])) {
          // number
          char num[intAsStrLen + 1] = "";

          size_t j = 0;
          while (i + j < len && isDigit(code[i + j])) {
            j++;
          }

          memcpy(num, &code[i], j);

          addToken(num, j, TokenType::Number);
          i += j;
        } else if (isAlpha(code[i])) {
          // identifier
          char ident[intAsStrLen + 1] = "";

          size_t j = 0;
          while (i + j < len && isAlpha(code[i + j])) {
            j++;
          }

          memcpy(ident, &code[i], j);

          addToken(ident, j, getIdentTokenType(ident));
          i += j;
        } else if (isSkippable(code[i])) {
          // skip
        } else {
          Serial.println("Undefined identifier found");
        }
        break;
    }
  }

  return tokens;
}



void Lexer::addToken(const char c[16], size_t tokLen, TokenType tokenType)
{
  memcpy((*currentToken).value, c, tokLen);
  currentToken->type = tokenType;
  currentToken++;
}

Lexer::TokenType Lexer::getIdentTokenType(const char* ident) {
  for (unsigned long long i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
    if (strcmp(keywords[i].value, ident) == 0) {
      return keywords[i].type;
    }
  }

  return Lexer::TokenType::Identifier;
}

bool Lexer::isSkippable(const char c) {
  return c == ' ' || c == '\n' || c == '\r';
}