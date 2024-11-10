#include "Lexer.h"


Lexer::Token* Lexer::tokenize(char* code, size_t len) {
  for (size_t i = 0; i < maxTokens; i++) {
    tokens[i] = Token();
  }
  currentToken = &tokens[0];

  for (size_t i = 0; i < len && currentState == LexerState::Running; i++) {
    const char c[2] = { code[i], '\0' };

    switch (code[i]) {
      case '(':
        addToken(currentToken, c, 2, TokenType::OpenParen);
        break;
      case ')':
        addToken(currentToken, c, 2, TokenType::CloseParen);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        addToken(currentToken, c, 2, TokenType::ArithmeticOperator);
        break;
      case '=':
        addToken(currentToken, c, 2, TokenType::Equals);
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
          Serial.print("Undefined identifier '");
          Serial.print(code[i]);
          Serial.println("' found");
          currentState = LexerState::UndefinedIdent;
          return tokens;
        }
        break;
    }
  }

  if(currentState != LexerState::TooManyTokens) {
    addToken(currentToken, "", tokLen, TokenType::EndOfFile);
  }
  
  return tokens;
}



void Lexer::addToken(Token* dest, const char src[tokLen], size_t srcLen, TokenType tokenType) {
  size_t copyLen = srcLen < tokLen - 1 ? srcLen : tokLen - 1;
  
  strncpy((*dest).value, src, copyLen);
  (*dest).value[copyLen] = '\0';
  (*dest).type = tokenType;

  if (dest == currentToken) {
    if(currentToken != &tokens[maxTokens]) {
      currentToken++;
    } else {
      Serial.println("Too many tokens in program!");
      currentState = LexerState::TooManyTokens;
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