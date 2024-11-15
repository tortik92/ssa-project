#include "Parser.h"

Parser::Program* Parser::produceAST(char* code, size_t len) {
  clearPools();
  tokensPtr = lexer.tokenize(code, len);

  while (!endOfFile()) {
    push(parseStmt());
  }

  return &program;
}

Parser::Stmt* Parser::parseStmt() {
  Serial.println("Parsing Statement!");
  return parseExpr();
}

Parser::Expr* Parser::parseExpr() {
  Serial.println("Parsing Expression!");
  return parseAdditiveExpr();
}

Parser::Expr* Parser::parseAdditiveExpr() {
  Serial.println("Parsing additive expression");
  Expr* leftMost = parseMultiplicativeExpr();


  while (strcmp(at()->value, "+") == 0 || strcmp(at()->value, "-") == 0) {
    char* op = eat()->value;

    Serial.print("Found ");
    Serial.println(op);

    Expr* right = parseMultiplicativeExpr();

    BinaryExpr* binaryExpr = newBinaryExpr();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    strncpy(binaryExpr->op, op, sizeof(binaryExpr->op));

    return binaryExpr;
  }

  return leftMost;
}

Parser::Expr* Parser::parseMultiplicativeExpr() {
  Serial.println("Parsing multiplicative expression");
  Expr* leftMost = parsePrimaryExpr();

  while (strcmp(at()->value, "*") == 0 || strcmp(at()->value, "/") == 0 || strcmp(at()->value, "%") == 0) {
    char* op = eat()->value;

    Serial.print("Found ");
    Serial.println(op);

    Expr* right = parsePrimaryExpr();

    BinaryExpr* binaryExpr = newBinaryExpr();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    strncpy(binaryExpr->op, op, sizeof(binaryExpr->op));

    return binaryExpr;
  }

  return leftMost;
}

Parser::Expr* Parser::parsePrimaryExpr() {
  const Lexer::TokenType tokenType = at()->type;

  switch (tokenType) {
    case Lexer::TokenType::Identifier:
    case Lexer::TokenType::Var:
    case Lexer::TokenType::If:
    case Lexer::TokenType::Else:
    case Lexer::TokenType::For:
    case Lexer::TokenType::While:
      {
        Identifier* identifier = newIdentifier();

        identifier->kind = NodeType::Identifier;
        strncpy(identifier->symbol, eat()->value, sizeof(identifier->symbol));

        Serial.print("Found identifier ");
        Serial.println(identifier->symbol);

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        NumericLiteral* number = newNumericLiteral();

        number->kind = NodeType::NumericLiteral;
        number->num = strtof(eat()->value, nullptr);

        Serial.print("Found numeric literal ");
        Serial.println(number->num);

        Serial.print("NextVal: ");
        Serial.print(at()->value);

        return number;
      }
    case Lexer::TokenType::OpenParen:
      {
        eat();  // consume '('
        Serial.println("Found '('");
        Expr* val = parseExpr();
        Serial.print("NextVal before expected: ");
        Serial.println(at()->value);
        expect(Lexer::TokenType::CloseParen, "Expected a ')', found \"");
        return val;
      }
    default:
      GlobalFunctions::restart("Unexpected token \"", at()->value, "\" found!");
      return nullptr;
  }
}

Lexer::Token* Parser::at() {
  return tokensPtr;
}

Lexer::Token* Parser::eat() {
  return tokensPtr++;
}

Lexer::Token* Parser::expect(Lexer::TokenType type, const char* err) {
  Lexer::Token* prev = eat();
  if (prev->type == Lexer::TokenType::Undefined || prev->type != type) {
    GlobalFunctions::restart(err, prev->value, "\"");
  }
  return prev;
}

bool Parser::endOfFile() {
  return tokensPtr->type == Lexer::TokenType::EndOfFile;
}

void Parser::push(Stmt* stmt) {
  program.body[currentStmtIndex] = stmt;

  if (currentStmtIndex < maxProgramStatements) {
    currentStmtIndex++;
  } else {
    GlobalFunctions::restart("Too many statements in program");
  }
}

void Parser::clearPools() {
  for(size_t i = 0; i < poolSize; i++) {
    binaryExprPool[i] = BinaryExpr();
    identifierPool[i] = Identifier();
    numericLiteralPool[i] = NumericLiteral();
  }

  binaryExprCount = 0;
  identifierCount = 0;
  numericLiteralCount = 0;
}

Parser::BinaryExpr* Parser::newBinaryExpr() {
  if (binaryExprCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for BinaryExpr nodes");
  }
  return &binaryExprPool[binaryExprCount++];
}

Parser::Identifier* Parser::newIdentifier() {
  if (identifierCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for Identifier nodes");
  }
  return &identifierPool[identifierCount++];
}

Parser::NumericLiteral* Parser::newNumericLiteral() {
  if (numericLiteralCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for NumericLiteral nodes");
  }
  return &numericLiteralPool[numericLiteralCount++];
}