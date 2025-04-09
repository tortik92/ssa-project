#include "Lexer.h"


std::queue<Lexer::Token> Lexer::tokenize(char* code, size_t len) {
  std::queue<Token> tokens;

  for (size_t i = 0; i < len; i++) {
    switch (code[i]) {
      case '(':
        addToken(&code[i], 1, TokenType::OpenParen, tokens);
        break;
      case ')':
        addToken(&code[i], 1, TokenType::CloseParen, tokens);
        break;
      case '[':
        addToken(&code[i], 1, TokenType::OpenBracket, tokens);
        break;
      case ']':
        addToken(&code[i], 1, TokenType::CloseBracket, tokens);
        break;
      case '{':
        addToken(&code[i], 1, TokenType::OpenBrace, tokens);
        break;
      case '}':
        addToken(&code[i], 1, TokenType::CloseBrace, tokens);
        break;
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
        if (code[i] == '-' && i + 1 < len && isDigit(code[i + 1])) {
          // negative number
          size_t numLen = 1;
          while (i + numLen < len && isDigit(code[i + numLen])) {
            numLen++;
          }

          addToken(&code[i], numLen, TokenType::Number, tokens);
          i += numLen - 1;
        } else {
          addToken(&code[i], 1, TokenType::ArithmeticOperator, tokens);
        }
        break;
      case '<':
      case '>':
        {
          uint8_t tokLen = i + 1 < len && code[i + 1] == '=' ? 2 : 1;
          addToken(&code[i], tokLen, TokenType::RelationalOperator, tokens);
          i += tokLen - 1;
        }
        break;
      case '!':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(&code[i], 2, TokenType::RelationalOperator, tokens);
          i++;
        } else {
          unrecognizedCharacter('!');
        }
        break;
      case '=':
        if (i + 1 < len && code[i + 1] == '=') {
          addToken(&code[i], 2, TokenType::RelationalOperator, tokens);
          i++;
        } else {
          addToken(&code[i], 1, TokenType::Equals, tokens);
        }
        break;
      case ';':
        addToken(&code[i], 1, TokenType::Semicolon, tokens);
        break;
      case ':':
        addToken(&code[i], 1, TokenType::Colon, tokens);
        break;
      case ',':
        addToken(&code[i], 1, TokenType::Comma, tokens);
        break;
      case '.':
        addToken(&code[i], 1, TokenType::Dot, tokens);
        break;
      case '"':
        {
          size_t strLen = 1;
          while (i + strLen < len && code[i + strLen] != '"') {
            strLen++;
          }

          if (i + strLen >= len || code[i + strLen] != '"') {
            ErrorHandler::reportError("Expected closing '\"' for string literal");
          }

          addToken(&code[i], strLen + 1, TokenType::StringLiteral, tokens);  // +1 for closing quote
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

          addToken(&code[i], numLen, TokenType::Number, tokens);
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

          addToken(&code[i], identLen, tokenType, tokens);
          i += identLen - 1;
        } else if (!isSpace(code[i]) && code[i] != '\0') {
          unrecognizedCharacter(code[i]);
          continue;
        }
        break;
    }
  }

  char endOfFile[] = "EOF";
  addToken(endOfFile, strlen(endOfFile), TokenType::EndOfFile, tokens);

  return tokens;
}


void Lexer::addToken(const char* src, size_t srcLen, TokenType tokenType, std::queue<Token>& tokens) {
  char* value = new char[srcLen + 1];
  strncpy(value, src, srcLen);
  value[srcLen] = '\0';

  tokens.push(Token(value, tokenType));
}

void Lexer::unrecognizedCharacter(char c) {
  char errMsg[30];
  Serial.print("Unrecognized character: ");
  Serial.print(c);
  Serial.print(" ");
  Serial.print(c, HEX);
  Serial.print(" ");
  Serial.println(c, DEC);

  snprintf(errMsg, 29, "Character \"%c\" not recognized", c);
  ErrorHandler::reportError(errMsg);
}