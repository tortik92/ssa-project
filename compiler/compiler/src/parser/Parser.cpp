#include "Parser.h"

Parser::Program* Parser::produceAST(char* code, size_t len) {
  cleanup();
  tokens = lexer->tokenize(code, len);

  GlobalFunctions::printMemoryStats("before producing AST");

  while (!endOfFile()) {
    push(parseStmt());
  }

  return &program;
}

Parser::Stmt* Parser::parseStmt() {
  Serial.println(at().value);
  switch (at().type) {
    case Lexer::TokenType::Let:
    case Lexer::TokenType::Const:
      Serial.println("Parsing var decl");
      return parseVarDeclaration();
    case Lexer::TokenType::If:
      Serial.println("Parsing if statement");
      return parseIfStmt();
    case Lexer::TokenType::While:
      Serial.println("Parsing while statement");
      return parseWhileStmt();
    case Lexer::TokenType::Break:
      Serial.println("Parsing break statement");
      return parseBreakStmt();
    default:
      Serial.println("Parsing expr");
      return parseExpr();
  }
}

Parser::VarDeclaration* Parser::parseVarDeclaration() {
  const bool isConstant = eat().type == Lexer::TokenType::Const;
  const char* ident = expect(Lexer::TokenType::Identifier, "identifier after let/const").value;

  VarDeclaration* varDecl = newVarDeclaration();
  varDecl->constant = isConstant;
  varDecl->ident = ident;

  if (at().type == Lexer::TokenType::Semicolon) {
    eat();
    if (isConstant) {
      GlobalFunctions::restart("Uninitialized const variable");
    }

    varDecl->value = nullptr;

    return varDecl;
  }

  expect(Lexer::TokenType::Equals, "=");
  varDecl->value = parseExpr();
  expect(Lexer::TokenType::Semicolon, ";");

  return varDecl;
}

Parser::IfStmt* Parser::parseIfStmt() {
  eat();  // consume 'if'
  IfStmt* ifStmt = newIfStmt();

  expect(Lexer::TokenType::OpenParen, "(");
  ifStmt->test = parseExpr();
  expect(Lexer::TokenType::CloseParen, ")");

  ifStmt->consequent = parseBlockStmt();
  ifStmt->alternate = nullptr;
  if (at().type == Lexer::TokenType::Else) {
    eat();  // consume 'else'
    ifStmt->alternate = parseBlockStmt();
  }

  return ifStmt;
}

Parser::WhileStmt* Parser::parseWhileStmt() {
  eat();  // consume 'while'
  WhileStmt* whileStmt = newWhileStmt();

  expect(Lexer::TokenType::OpenParen, "(");
  whileStmt->test = parseExpr();
  expect(Lexer::TokenType::CloseParen, ")");

  whileStmt->body = parseBlockStmt();

  return whileStmt;
}

Parser::BreakStmt* Parser::parseBreakStmt() {
  eat(); // consume 'break'
  Serial.println("Parsing break statement");
  BreakStmt* breakStmt = newBreakStmt();

  return breakStmt;
}

Parser::BlockStmt* Parser::parseBlockStmt() {
  expect(Lexer::TokenType::OpenBrace, "{");
  BlockStmt* blockStmt = newBlockStmt();

  size_t lineCount = 0;
  while (at().type != Lexer::TokenType::CloseBrace) {
    if (endOfFile()) {
      GlobalFunctions::restart("Reached end of file, expected '}'");
    }
    if (lineCount >= maxBlockStatements) {
      char errmsg[64];
      sprintf(errmsg, "Maximum of %d statements in {} exceeded", maxBlockStatements);
      GlobalFunctions::restart(errmsg);
    }

    blockStmt->body[lineCount] = parseStmt();
    lineCount++;
  }

  eat();  // consume '}'

  return blockStmt;
}

Parser::Expr* Parser::parseExpr() {
  return parseLogicalExpr();
}

Parser::Expr* Parser::parseLogicalExpr() {
  Expr* left = parseRelationalExpr();

  while (strcmp(at().value, "and") == 0 || strcmp(at().value, "or") == 0) {
    char* op = eat().value;

    Expr* right = parseRelationalExpr();

    LogicalExpr* logicalExpr = newLogicalExpr();
    logicalExpr->op = op;
    logicalExpr->left = left;
    logicalExpr->right = right;

    return logicalExpr;
  }

  return left;
}

Parser::Expr* Parser::parseRelationalExpr() {
  Expr* left = parseAssignmentExpr();

  while (strcmp(at().value, "<") == 0 || strcmp(at().value, "<=") == 0 || strcmp(at().value, ">") == 0 || strcmp(at().value, ">=") == 0 || strcmp(at().value, "==") == 0 || strcmp(at().value, "!=") == 0) {
    char* op = eat().value;

    Expr* right = parseAssignmentExpr();

    BinaryExpr* relationalExpr = newBinaryExpr();
    relationalExpr->op = op;
    relationalExpr->left = left;
    relationalExpr->right = right;

    return relationalExpr;
  }

  return left;
}

Parser::Expr* Parser::parseAssignmentExpr() {
  Expr* left = parseAdditiveExpr();

  if (at().type == Lexer::TokenType::Equals) {
    eat();
    if (left->kind != NodeType::Identifier) {
      GlobalFunctions::restart("Expected variable name for assignment");
    }
    Expr* value = parseAssignmentExpr();

    AssignmentExpr* assignmentExpr = newAssignmentExpr();
    assignmentExpr->assignee = left;
    assignmentExpr->value = value;

    return assignmentExpr;
  }

  return left;
}

Parser::Expr* Parser::parseAdditiveExpr() {
  Expr* leftMost = parseMultiplicativeExpr();


  while (strcmp(at().value, "+") == 0 || strcmp(at().value, "-") == 0) {
    char* op = eat().value;

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
  Expr* leftMost = parseCallMemberExpr();

  while (strcmp(at().value, "*") == 0 || strcmp(at().value, "/") == 0 || strcmp(at().value, "%") == 0) {
    char* op = eat().value;

    Expr* right = parseCallMemberExpr();

    BinaryExpr* binaryExpr = newBinaryExpr();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    binaryExpr->op = op;

    leftMost = binaryExpr;
  }

  return leftMost;
}

Parser::Expr* Parser::parseCallMemberExpr() {
  Expr* member = parsePrimaryExpr();

  if (at().type == Lexer::TokenType::OpenParen) {
    Serial.println("Found OpenParen");
    return parseCallExpr(member);
  }

  return member;
}

Parser::Expr* Parser::parseCallExpr(Expr* caller) {
  Serial.println("Parsing call expr");
  CallExpr* callExpr = newCallExpr();
  callExpr->caller = caller;
  parseArgs(callExpr);

  if (at().type == Lexer::TokenType::OpenParen) {
    Serial.println("Parsing CallExpr in CallExpr");
    parseCallExpr(callExpr);
  }

  return callExpr;
}

void Parser::parseArgs(CallExpr* callExpr) {
  expect(Lexer::TokenType::OpenParen, "(");
  parseArgsList(callExpr);
  expect(Lexer::TokenType::CloseParen, ")");
}

void Parser::parseArgsList(CallExpr* callExpr) {
  callExpr->args[0] = strcmp(at().value, ")") != 0 ? parseAssignmentExpr() : nullptr;

  for (size_t i = 1; at().type == Lexer::TokenType::Comma; i++) {
    if (i >= maxFunctionArgs) {
      GlobalFunctions::restart("Function argument list exceeds argument count limit");
    }

    eat();
    callExpr->args[i] = parseAssignmentExpr();
    Serial.println("Parsing argument from argument list");
    if (callExpr->args[i]->kind == NodeType::NumericLiteral) Serial.println(static_cast<NumericLiteral*>(callExpr->args[i])->num);
  }
}

Parser::Expr* Parser::parsePrimaryExpr() {
  const Lexer::TokenType tokenType = at().type;

  switch (tokenType) {
    case Lexer::TokenType::Identifier:
    case Lexer::TokenType::Let:
    case Lexer::TokenType::Const:
    case Lexer::TokenType::If:
    case Lexer::TokenType::Else:
    case Lexer::TokenType::While:
    case Lexer::TokenType::Break:
    case Lexer::TokenType::LogicalOperator:
      {
        Identifier* identifier = newIdentifier();

        identifier->symbol = eat().value;

        Serial.print("Found identifier ");
        Serial.println(identifier->symbol);

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        NumericLiteral* number = newNumericLiteral();

        number->num = strtof(eat().value, nullptr);

        Serial.print("Found number ");
        Serial.println(number->num);

        return number;
      }
    case Lexer::TokenType::OpenParen:
      {
        eat();  // consume '('
        Expr* val = parseExpr();

        expect(Lexer::TokenType::CloseParen, ")");
        return val;
      }
    default:
      GlobalFunctions::restart("Unexpected token \"", at().value, "\" found!");
      return nullptr;
  }
}

Lexer::Token Parser::at() {
  if (tokens.empty()) {
    GlobalFunctions::restart("Trying to access empty tokens queue");
  }
  return tokens.front();
}

Lexer::Token Parser::eat() {
  Lexer::Token returnValue = tokens.front();
  tokens.pop();
  return returnValue;
}

Lexer::Token Parser::expect(Lexer::TokenType type, const char* expectedVal) {
  Lexer::Token prev = eat();
  if (prev.type != type) {
    GlobalFunctions::restart(expectedVal, (const char*)prev.value);
  }
  return prev;
}

bool Parser::endOfFile() {
  return tokens.front().type == Lexer::TokenType::EndOfFile;
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
  for (size_t i = 0; i < maxProgramStatements; i++) {
    program.body[i] = nullptr;
  }
  currentStmtIndex = 0;

  for (size_t i = 0; i < poolSize; i++) {
    binaryExprPool[i] = BinaryExpr();
    identifierPool[i] = Identifier();
    varDeclarationPool[i] = VarDeclaration();
    numericLiteralPool[i] = NumericLiteral();
    assignmentExprPool[i] = AssignmentExpr();
    callExprPool[i] = CallExpr();
    blockStmtPool[i] = BlockStmt();
    ifStmtPool[i] = IfStmt();
    whileStmtPool[i] = WhileStmt();
    breakStmtPool[i] = BreakStmt();
  }

  binaryExprCount = 0;
  identifierCount = 0;
  varDeclarationCount = 0;
  numericLiteralCount = 0;
  assignmentExprCount = 0;
  callExprCount = 0;
  blockStmtCount = 0;
  ifStmtCount = 0;
  whileStmtCount = 0;
  breakStmtCount = 0;
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

Parser::VarDeclaration* Parser::newVarDeclaration() {
  if (varDeclarationCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for VarDeclaration nodes");
  }
  return &varDeclarationPool[varDeclarationCount++];
}

Parser::AssignmentExpr* Parser::newAssignmentExpr() {
  if (assignmentExprCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for BinaryExpr nodes");
  }
  return &assignmentExprPool[assignmentExprCount++];
}

Parser::CallExpr* Parser::newCallExpr() {
  if (callExprCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for CallExpr nodes");
  }
  return &callExprPool[callExprCount++];
}

Parser::LogicalExpr* Parser::newLogicalExpr() {
  if (logicalExprCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for LogicalExpr nodes");
  }
  return &logicalExprPool[logicalExprCount++];
}

Parser::BlockStmt* Parser::newBlockStmt() {
  if (blockStmtCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for BlockStmt nodes");
  }
  return &blockStmtPool[blockStmtCount++];
}

Parser::IfStmt* Parser::newIfStmt() {
  if (ifStmtCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for IfStmt nodes");
  }
  return &ifStmtPool[ifStmtCount++];
}

Parser::WhileStmt* Parser::newWhileStmt() {
  if (whileStmtCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for WhileStmt nodes");
  }
  return &whileStmtPool[whileStmtCount++];
}

Parser::BreakStmt* Parser::newBreakStmt() {
  if (breakStmtCount >= poolSize) {
    GlobalFunctions::restart("Out of memory for BreakStmt nodes");
  }
  return &breakStmtPool[breakStmtCount++];
}