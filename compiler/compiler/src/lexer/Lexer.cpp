#include "Lexer.h"


std::queue<Lexer::Token> Lexer::tokenize(char* code, size_t len) {
  // clean up for fresh tokens
  while(!tokens.empty()) {
    tokens.pop();
  }

  Serial.println("Cleaned up tokens");

  for (size_t i = 0; i < len; i++) {
    switch (code[i]) {
      case '(':
        addToken(&code[i], 1, TokenType::OpenParen);
        break;
      case ')':
        addToken(&code[i], 1, TokenType::CloseParen);
        break;
      case '[':
        addToken(&code[i], 1, TokenType::OpenBracket);
        break;
      case ']':
        addToken(&code[i], 1, TokenType::CloseBracket);
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
          ErrorHandler::restart("Character '!' not recognized");
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
      case ':':
        addToken(&code[i], 1, TokenType::Colon);
        break;
      case ',':
        addToken(&code[i], 1, TokenType::Comma);
        break;
      case '.':
        addToken(&code[i], 1, TokenType::Dot);
        break;
      case '"':
        {
          size_t strLen = 1;
          while(i + strLen < len && code[i + strLen] != '"') {
            strLen++;
          }

          if(i + strLen >= len || code[i + strLen] != '"') {
            ErrorHandler::restart("Expected closing '\"' for string literal");
          }

          addToken(&code[i], strLen + 1, TokenType::StringLiteral); // +1 for closing quote
          i += strLen;
          break;
        }
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
        } else if (isAlpha(code[i]) || code[i] == '$' || code[i] == '_') {
          // identifier
          size_t identLen = 1;
          while (i + identLen < len && (isAlphaNumeric(code[i + identLen]) || code[i + identLen] == '_' || code[i + identLen] == '$')) {
            identLen++;
          }

          TokenType tokenType = TokenType::Identifier;
          for (size_t j = 0; j < keywordCount; j++) {
            if (strlen(keywords[j].value) == identLen && strncmp(keywords[j].value, &code[i], identLen) == 0) {
              tokenType = keywords[j].type;
              Serial.println(keywords[j].value);
              break;
            }
          }

          addToken(&code[i], identLen, tokenType);
          i += identLen - 1;
        } else if (!isSpace(code[i]) && code[i] != '\0') {
          ErrorHandler::restart("Character \"", &code[i], "\" not recognized");
          return tokens;
        }
        break;
    }
  }

  char endOfFile[] = "EOF";
  addToken(endOfFile, strlen(endOfFile), TokenType::EndOfFile);

  return std::move(tokens);
}


void Lexer::addToken(const char* src, size_t srcLen, TokenType tokenType) {
  char* value = new char[srcLen + 1];
  strncpy(value, src, srcLen);
  value[srcLen] = '\0';

  if (tokens.size() < maxTokens) {
    tokens.push(Token(value, tokenType));
  } else {
    ErrorHandler::restart("Too many tokens in program!");
  }
}