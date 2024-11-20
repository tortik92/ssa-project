#include "Parser.h"

Parser::Program* Parser::produceAST(char* code, size_t len) {
  cleanup();
  tokensPtr = lexer->tokenize(code, len);

  while (!endOfFile()) {
    push(parseStmt());
  }

  return &program;
}

Parser::Stmt* Parser::parseStmt() {
  Serial.println(at()->value);
  switch (this->at()->type)
  {
  case Lexer::TokenType::Let:
  case Lexer::TokenType::Const:
    Serial.println("Parsing var decl");
    return parseVarDeclaration();
  default:
    Serial.println("Parsing expr");
    return parseExpr();
  }
}

Parser::VarDeclaration* Parser::parseVarDeclaration() {
  const bool isConstant = eat()->type == Lexer::TokenType::Const;
  const char* ident = expect(Lexer::TokenType::Identifier, "Expected identifier after let/const")->value;

  VarDeclaration* varDecl = newVarDeclaration();
  varDecl->constant = isConstant;
  varDecl->ident = ident;

  if(this->at()->type == Lexer::TokenType::Semicolon) {
    if(isConstant) {
      GlobalFunctions::restart("Uninitialized const variable");
    }

    varDecl->value = nullptr;

    return varDecl;
  }

  expect(Lexer::TokenType::Equals, "Expected '='");
  varDecl->value = parseExpr();
  expect(Lexer::TokenType::Semicolon, "Expected ';'");

  return varDecl;
}

Parser::Expr* Parser::parseExpr() {
  return parseAdditiveExpr();
}

Parser::Expr* Parser::parseAdditiveExpr() {
  Expr* leftMost = parseMultiplicativeExpr();


  while (strcmp(at()->value, "+") == 0 || strcmp(at()->value, "-") == 0) {
    char op = eat()->value[0];

    Expr* right = parseMultiplicativeExpr();

    BinaryExpr* binaryExpr = newBinaryExpr();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    binaryExpr->op = op;

    leftMost = binaryExpr;
  }

  return leftMost;
}

Parser::Expr* Parser::parseMultiplicativeExpr() {
  Expr* leftMost = parsePrimaryExpr();

  while (strcmp(at()->value, "*") == 0 || strcmp(at()->value, "/") == 0 || strcmp(at()->value, "%") == 0) {
    char op = eat()->value[0];

    Expr* right = parsePrimaryExpr();

    BinaryExpr* binaryExpr = newBinaryExpr();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    binaryExpr->op = op;

    leftMost = binaryExpr;
  }

  return leftMost;
}

Parser::Expr* Parser::parsePrimaryExpr() {
  const Lexer::TokenType tokenType = at()->type;

  switch (tokenType) {
    case Lexer::TokenType::Identifier:
    case Lexer::TokenType::Let:
    case Lexer::TokenType::Const:
    case Lexer::TokenType::If:
    case Lexer::TokenType::Else:
    case Lexer::TokenType::For:
    case Lexer::TokenType::While:
      {
        Identifier* identifier = newIdentifier();

        identifier->kind = NodeType::Identifier;
        identifier->symbol = eat()->value;

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        NumericLiteral* number = newNumericLiteral();

        number->kind = NodeType::NumericLiteral;
        number->num = strtof(eat()->value, nullptr);

        return number;
      }
    case Lexer::TokenType::OpenParen:
      {
        eat();  // consume '('
        Expr* val = parseExpr();

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
  if (prev->type != type) {
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

void Parser::cleanup() {
  for(size_t i = 0; i < maxProgramStatements; i++) {
    program.body[i] = nullptr;
  }
  currentStmtIndex = 0;

  for (size_t i = 0; i < poolSize; i++) {
    binaryExprPool[i] = BinaryExpr();
    identifierPool[i] = Identifier();
    varDeclarationPool[i] = VarDeclaration();
    numericLiteralPool[i] = NumericLiteral();
  }

  binaryExprCount = 0;
  identifierCount = 0;
  varDeclarationCount = 0;
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

Parser::VarDeclaration* Parser::newVarDeclaration() {
  if (varDeclarationCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for VarDeclaration nodes");
  }
  return &varDeclarationPool[varDeclarationCount++];
}

Parser::NumericLiteral* Parser::newNumericLiteral() {
  if (numericLiteralCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for NumericLiteral nodes");
  }
  return &numericLiteralPool[numericLiteralCount++];
}