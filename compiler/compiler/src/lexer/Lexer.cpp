#include "Lexer.h"


std::queue<std::shared_ptr<Lexer::Token>> Lexer::tokenize(char* code, size_t len) {
  while (!tokens.empty()) {
    tokens.pop();
  }
  
  for (size_t i = 0; i < len; i++) {
    switch (code[i]) {
      case '(':
        addToken(&code[i], 1, TokenType::OpenParen);
        break;
      case ')':
        addToken(&code[i], 1, TokenType::CloseParen);
        break;
      case '{':
        addToken(&code[i], 1, TokenType::OpenBrace);
        break;
      case '}':
        addToken(&code[i], 1, TokenType::CloseBrace);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        addToken(&code[i], 1, TokenType::ArithmeticOperator);
        break;
      case '<':
      case '>':
        {
          uint8_t tokLen = i + 1 < len && code[i + 1] == '=' ? 2 : 1;
          addToken(&code[i], tokLen, TokenType::RelationalOperator);
          i += tokLen - 1;
        }
        break;
      case '!':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(&code[i], 2, TokenType::RelationalOperator);
          i++;
        } else {
          GlobalFunctions::restart("Character '!' not recognized");
        }
        break;
      case '=':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(&code[i], 2, TokenType::RelationalOperator);
          i++;
        } else {
          addToken(&code[i], 1, TokenType::Equals);
        }
        break;
      case ';':
        addToken(&code[i], 1, TokenType::Semicolon);
        break;
      case ',':
        addToken(&code[i], 1, TokenType::Comma);
        break;
      default:
        // handle multicharacter tokens
        if (isDigit(code[i])) {
          // number
          size_t numLen = 1;
          while (i + numLen < len && isDigit(code[i + numLen])) {
            numLen++;
          }

          addToken(&code[i], numLen, TokenType::Number);
          i += numLen - 1;
        } else if (isAlpha(code[i])) {
          // identifier

          size_t identLen = 1;
          while (i + identLen < len && isAlphaNumeric(code[i + identLen])) {
            identLen++;
          }

          addToken(&code[i], identLen, getIdentTokenType(&code[i], identLen));
          i += identLen - 1;
        } else if (!isSpace(code[i]) && code[i] != '\0') {
          char err[50];
          snprintf(err, 49, "Character \"%s\" not recognized", &code[i]);
          GlobalFunctions::restart(err);
          return tokens;
        }
        break;
    }
  }

  char endOfFile[] = "EOF";

  addToken(endOfFile, strlen(endOfFile), TokenType::EndOfFile);

  Serial.println("Front in Lexer: ");
  Serial.println(tokens.front().get()->value);

  return tokens;
}


void Lexer::addToken(const char* src, size_t srcLen, TokenType tokenType) {
  char* value = new char[srcLen + 1];
  strncpy(value, src, srcLen);
  value[srcLen] = '\0';
  
  if(tokens.size() < maxTokens) {
    tokens.push(std::make_shared<Token>(value, tokenType));
  } else {
    GlobalFunctions::restart("Too many tokens in program!");
  }
}

Lexer::TokenType Lexer::getIdentTokenType(const char* ident, size_t len) {
  for (size_t i = 0; i < keywordCount; i++) {
    if (strlen(keywords[i].value) == len && strncmp(keywords[i].value, ident, len) == 0) {
      return keywords[i].type;
    }
  }

  return Lexer::TokenType::Identifier;
}