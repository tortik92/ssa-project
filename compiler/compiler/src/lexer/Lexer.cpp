#include "Lexer.h"


Lexer::Token* Lexer::tokenize(char* code, size_t len) {
  for (size_t i = 0; i < maxTokens && tokens[i].value != nullptr; i++) {
    delete[] tokens[i].value;
    tokens[i] = Token();
  }
  currentToken = &tokens[0];

  for (size_t i = 0; i < len; i++) {
    switch (code[i]) {
      case '(':
        addToken(currentToken, &code[i], 1, TokenType::OpenParen);
        break;
      case ')':
        addToken(currentToken, &code[i], 1, TokenType::CloseParen);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        addToken(currentToken, &code[i], 1, TokenType::ArithmeticOperator);
        break;
      case '<':
      case '>':
        addToken(currentToken, &code[i], i + 1 < len && code[i + 1] == '=' ? 2 : 1, TokenType::RelationalOperator);
        break;
      case '!':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(currentToken, &code[i], 2, TokenType::RelationalOperator);
        } else {
          addToken(currentToken, &code[i], 1, TokenType::LogicalOperator);
        }
        break;
      case '&':
      case '|':
        if(i + 1 >= len || code[i + 1] != code[i]) {
          GlobalFunctions::restart("Expected '", &code[i], "'");
        } else {
          addToken(currentToken, &code[i], 2, TokenType::LogicalOperator);
        }
        break;
      case '=':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(currentToken, &code[i], 2, TokenType::RelationalOperator);
        } else {
          addToken(currentToken, &code[i], 1, TokenType::Equals);
        }
        break;
      case ';':
        addToken(currentToken, &code[i], 1, TokenType::Semicolon);
        break;
      case ',':
        addToken(currentToken, &code[i], 1, TokenType::Comma);
        break;
      case '.':
        addToken(currentToken, &code[i], 1, TokenType::Dot);
        break;
      default:
        // handle multicharacter tokens
        if (isDigit(code[i])) {
          // number
          size_t numLen = 1;
          while (i + numLen < len && isDigit(code[i + numLen])) {
            numLen++;
          }

          addToken(currentToken, &code[i], numLen, TokenType::Number);
          i += numLen - 1;
        } else if (isAlpha(code[i])) {
          // identifier

          size_t identLen = 1;
          while (i + identLen < len && isAlphaNumeric(code[i + identLen])) {
            identLen++;
          }

          addToken(currentToken, &code[i], identLen, getIdentTokenType(&code[i], identLen));
          i += identLen - 1;
        } else if (!isSpace(code[i]) && code[i] != '\0') {
          GlobalFunctions::restart("Character \"", &code[i], "\" not recognized");
          return tokens;
        }
        break;
    }
  }

  char endOfFile[] = "EOF";

  addToken(currentToken, endOfFile, strlen(endOfFile), TokenType::EndOfFile);

  return tokens;
}


void Lexer::addToken(Token* dest, const char* src, size_t srcLen, TokenType tokenType) {
  dest->value = new char[srcLen + 1];
  strncpy(dest->value, src, srcLen);
  dest->value[srcLen] = '\0';
  dest->type = tokenType;

  if (dest == currentToken) {
    if (currentToken != &tokens[maxTokens]) {
      currentToken++;
    } else {
      GlobalFunctions::restart("Too many tokens in program!");
    }
  }
}

Lexer::TokenType Lexer::getIdentTokenType(const char* ident, size_t len) {
  for (size_t i = 0; i < keywordCount; i++) {
    if (strncmp(keywords[i].value, ident, len) == 0) {
      return keywords[i].type;
    }
  }

  return Lexer::TokenType::Identifier;
}