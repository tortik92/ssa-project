#include "Parser.h"

Parser::Program* Parser::produceAST(char* code, size_t len) {
  Serial.println("Before cleanup");
  cleanup();
  Serial.println("After cleanup");
  tokens = lexer->tokenize(code, len);
  Serial.println("Tokenized");

  ErrorHandler::printMemoryStats("before producing AST");

  while (!endOfFile()) {
    push(parseStmt());
  }

  program.body.shrink_to_fit();

  return &program;
}

Parser::Stmt* Parser::parseStmt() {
  Serial.println("parseStmt");
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
  Serial.println("parseVarDeclaration");
  const bool isConstant = eat().type == Lexer::TokenType::Const;
  Lexer::Token varName = expect(Lexer::TokenType::Identifier, "identifier after let/const");

  VarDeclaration* varDecl = varDeclarationPool.allocate();
  varDecl->constant = isConstant;
  varDecl->ident = strcpyNew(varName.value);
  Serial.println(varDecl->ident);

  if (at().type == Lexer::TokenType::Semicolon) {
    eat();
    if (isConstant) {
      ErrorHandler::restart("Uninitialized const variable");
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
  Serial.println("parseIfStmt");
  eat();  // consume 'if'
  IfStmt* ifStmt = ifStmtPool.allocate();

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
  Serial.println("parseWhileStmt");
  eat();  // consume 'while'
  WhileStmt* whileStmt = whileStmtPool.allocate();

  expect(Lexer::TokenType::OpenParen, "(");
  whileStmt->test = parseExpr();
  expect(Lexer::TokenType::CloseParen, ")");

  whileStmt->body = parseBlockStmt();

  return whileStmt;
}

Parser::BreakStmt* Parser::parseBreakStmt() {
  Serial.println("parseBreakStmt");
  eat();  // consume 'break'
  BreakStmt* breakStmt = breakStmtPool.allocate();
  expect(Lexer::TokenType::Semicolon, ";");
  return breakStmt;
}

Parser::BlockStmt* Parser::parseBlockStmt() {
  Serial.println("parseBlockStmt");
  expect(Lexer::TokenType::OpenBrace, "{");
  BlockStmt* blockStmt = blockStmtPool.allocate();
  blockStmt->body.reserve(estimatedBlockStatements);

  while (at().type != Lexer::TokenType::CloseBrace) {
    if (endOfFile()) {
      ErrorHandler::restart("Reached end of file, expected '}'");
    }

    blockStmt->body.push_back(parseStmt());
  }
  blockStmt->body.shrink_to_fit();

  eat();  // consume '}'

  return blockStmt;
}

Parser::Expr* Parser::parseExpr() {
  Serial.println("parseExpr");
  return parseLogicalExpr();
}

Parser::Expr* Parser::parseLogicalExpr() {
  Serial.println("parseLogicalExpr");
  Expr* left = parseRelationalExpr();

  while (strcmp(at().value, "and") == 0 || strcmp(at().value, "or") == 0) {
    Lexer::Token op = eat();

    Expr* right = parseRelationalExpr();

    LogicalExpr* logicalExpr = logicalExprPool.allocate();
    logicalExpr->left = left;
    logicalExpr->right = right;
    logicalExpr->op = strcpyNew(op.value);

    return logicalExpr;
  }

  Serial.println("return parseLogicalExpr");

  return left;
}

Parser::Expr* Parser::parseRelationalExpr() {
  Serial.println("parseRelationalExpr");
  Expr* left = parseAssignmentExpr();

  while (strcmp(at().value, "<") == 0 || strcmp(at().value, "<=") == 0 || strcmp(at().value, ">") == 0 || strcmp(at().value, ">=") == 0 || strcmp(at().value, "==") == 0 || strcmp(at().value, "!=") == 0) {
    Lexer::Token op = eat();

    Expr* right = parseAssignmentExpr();

    BinaryExpr* relationalExpr = binaryExprPool.allocate();
    relationalExpr->left = left;
    relationalExpr->right = right;
    relationalExpr->op = strcpyNew(op.value);

    return relationalExpr;
  }

  Serial.println("return parseRelationalExpr");

  return left;
}

Parser::Expr* Parser::parseAssignmentExpr() {
  Serial.println("parseAssignmentExpr");
  Expr* left = parseAdditiveExpr();

  if (at().type == Lexer::TokenType::Equals) {
    eat();
    if (left->kind != NodeType::Identifier) {
      ErrorHandler::restart("Expected variable name for assignment");
    }
    Expr* value = parseAssignmentExpr();

    AssignmentExpr* assignmentExpr = assignmentExprPool.allocate();
    assignmentExpr->assignee = left;
    assignmentExpr->value = value;

    expect(Lexer::TokenType::Semicolon, ";");

    return assignmentExpr;
  }

  Serial.println("return parseAssignmentExpr");

  return left;
}

Parser::Expr* Parser::parseAdditiveExpr() {
  Serial.println("parseAdditiveExpr");
  Expr* leftMost = parseMultiplicativeExpr();


  while (strcmp(at().value, "+") == 0 || strcmp(at().value, "-") == 0) {
    Lexer::Token op = eat();

    Expr* right = parseMultiplicativeExpr();

    BinaryExpr* binaryExpr = binaryExprPool.allocate();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    binaryExpr->op = strcpyNew(op.value);

    leftMost = binaryExpr;
  }

  Serial.println("return parseAdditiveExpr");

  return leftMost;
}

Parser::Expr* Parser::parseMultiplicativeExpr() {
  Serial.println("parseMultiplicativeExpr");
  Expr* leftMost = parseCallMemberExpr();

  while (strcmp(at().value, "*") == 0 || strcmp(at().value, "/") == 0 || strcmp(at().value, "%") == 0) {
    Lexer::Token op = eat();

    Expr* right = parseCallMemberExpr();

    BinaryExpr* binaryExpr = binaryExprPool.allocate();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = leftMost;
    binaryExpr->right = right;
    binaryExpr->op = strcpyNew(op.value);

    leftMost = binaryExpr;
  }

  Serial.println("return parseMultiplicativeExpr");

  return leftMost;
}

Parser::Expr* Parser::parseCallMemberExpr() {
  Serial.println("parseCallMemberExpr");
  Expr* member = parsePrimaryExpr();

  if (at().type == Lexer::TokenType::OpenParen) {
    Serial.println("Found OpenParen");
    return parseCallExpr(member);
  }

  Serial.println("return parseCallMemberExpr");

  return member;
}

Parser::Expr* Parser::parseCallExpr(Expr* caller) {
  Serial.println("parseCallExpr");
  CallExpr* callExpr = callExprPool.allocate();
  callExpr->caller = caller;
  parseArgs(callExpr);
  expect(Lexer::TokenType::Semicolon, ";");

  return callExpr;
}

void Parser::parseArgs(CallExpr* callExpr) {
  Serial.println("parseArgs");
  expect(Lexer::TokenType::OpenParen, "(");
  if (strcmp(at().value, ")") != 0) {
    parseArgsList(callExpr);
  }
  expect(Lexer::TokenType::CloseParen, ")");
}

void Parser::parseArgsList(CallExpr* callExpr) {
  Serial.println("parseArgsList");
  Expr* firstParam = parseExpr();
  callExpr->args.push_back(firstParam);

  while (at().type == Lexer::TokenType::Comma) {
    eat();
    callExpr->args.push_back(parseExpr());
  }
}

Parser::Expr* Parser::parsePrimaryExpr() {
  Serial.println("parsePrimaryExpr");

  switch (at().type) {
    case Lexer::TokenType::Identifier:
    case Lexer::TokenType::Let:
    case Lexer::TokenType::Const:
    case Lexer::TokenType::If:
    case Lexer::TokenType::Else:
    case Lexer::TokenType::While:
    case Lexer::TokenType::Break:
    case Lexer::TokenType::LogicalOperator:
      {
        Identifier* identifier = identifierPool.allocate();

        Lexer::Token identToken = eat();

        identifier->symbol = strcpyNew(identToken.value);

        Serial.print("Found identifier ");
        Serial.println(identifier->symbol);

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        NumericLiteral* number = numericLiteralPool.allocate();

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
      ErrorHandler::restart("Unexpected token \"", at().value, "\" found!");
      return nullptr;
  }
}

Lexer::Token Parser::at() {
  if (tokens.empty()) {
    ErrorHandler::restart("Trying to access empty tokens queue");
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
    ErrorHandler::restart(expectedVal, (const char*)prev.value);
  }
  Serial.print("Found expected '");
  Serial.print(prev.value);
  Serial.println("'");
  return prev;
}

bool Parser::endOfFile() {
  return tokens.front().type == Lexer::TokenType::EndOfFile;
}

void Parser::push(Stmt* stmt) {
  program.body.push_back(stmt);

  if (currentStmtIndex < estimatedProgramStatements) {
    currentStmtIndex++;
  } else {
    ErrorHandler::restart("Too many statements in program");
  }
}

void Parser::cleanup() {
  program.body.clear();
  Serial.println("Program body cleared");
  currentStmtIndex = 0;

  binaryExprPool.cleanup();
  identifierPool.cleanup();
  varDeclarationPool.cleanup();
  numericLiteralPool.cleanup();
  assignmentExprPool.cleanup();
  callExprPool.cleanup();
  blockStmtPool.cleanup();
  ifStmtPool.cleanup();
  whileStmtPool.cleanup();
  breakStmtPool.cleanup();
  Serial.println("End of cleanup function");
}

char* Parser::strcpyNew(const char* src) {
  size_t len = strlen(src) + 1; // one longer than string for null termination
  char* dest = new char[len];
  strncpy(dest, src, len);
  return dest;
}

// toString
void Parser::toStringNumericLiteral(const Parser::NumericLiteral* numLit) {
  Serial.print("{\"type\":\"numericLiteral\",\"value\":\"");
  Serial.print(numLit->num);
  Serial.print("\"}");
}

void Parser::toStringIdentifier(const Parser::Identifier* ident) {
  Serial.print("{\"type\":\"identifier\",\"symbol\":\"");
  Serial.print(ident->symbol);
  Serial.print("\"}");
}

void Parser::toStringBinaryExpr(const Parser::BinaryExpr* binaryExpr) {
  Serial.print("{\"type\":\"binaryExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt*>(binaryExpr->left));
  Serial.print(",\"operator\":\"");
  Serial.print(binaryExpr->op);
  Serial.print("\",");
  Serial.print("\"right\":");
  toString(static_cast<const Parser::Stmt*>(binaryExpr->right));
  Serial.print("}");
}

void Parser::toStringLogicalExpr(const Parser::LogicalExpr* logicalExpr) {
  Serial.print("{\"type\":\"logicalExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt*>(logicalExpr->left));
  Serial.print(",\"operator\":\"");
  Serial.print(logicalExpr->op);
  Serial.print("\",\"right\":");
  toString(static_cast<const Parser::Stmt*>(logicalExpr->right));
  Serial.print("}");
}

void Parser::toStringVarDecl(const Parser::VarDeclaration* varDecl) {
  Serial.print("{\"type\":\"varDecl\",");
  Serial.print("\"isConstant\":");
  Serial.print(varDecl->constant ? "\"true\"," : "\"false\",");
  Serial.print("\"identifier\":\"");
  Serial.print(varDecl->ident);
  Serial.print("\",\"value\":");
  if (varDecl->value != nullptr) toString(varDecl->value);
  Serial.print("}");
}

void Parser::toStringBlockStmt(const Parser::BlockStmt* blockStmt) {
  Serial.print("{\"type\":\"blockStmt\",\"body\":[");
  for (size_t i = 0; i < blockStmt->body.size(); i++) {
    toString(blockStmt->body[i]);
    Serial.print(",");
  }
  Serial.print("]}");
}

void Parser::toStringIfStmt(const Parser::IfStmt* ifStmt) {
  Serial.print("{\"type\":\"ifStmt\",");
  Serial.print("\"test\":");
  toString(ifStmt->test);
  Serial.print(",\"consequent\":");
  toStringBlockStmt(ifStmt->consequent);
  Serial.print(",\"alternate\":");
  if (ifStmt->alternate != nullptr) {
    toStringBlockStmt(ifStmt->alternate);
  } else {
    Serial.print("null");
  }
  Serial.print("}");
}

void Parser::toStringWhileStmt(const Parser::WhileStmt* whileStmt) {
  Serial.print("{\"type\":\"whileStmt\",");
  Serial.print("\"test\":");
  toString(whileStmt->test);
  Serial.print(",\"body\":");
  toStringBlockStmt(whileStmt->body);
  Serial.print("}");
}

void Parser::toStringBreakStmt(const Parser::BreakStmt* breakStmt) {
  Serial.print("{\"type\":\"breakStmt\"}");
}

void Parser::toStringAssignmentExpr(const Parser::AssignmentExpr* assignmentExpr) {
  Serial.print("{\"type\":\"assignmentExpr\",\"assignee\":");
  toString(assignmentExpr->assignee);
  Serial.print(",\"value\":");
  toString(assignmentExpr->value);
  Serial.print("}");
}

void Parser::toStringCallExpr(const Parser::CallExpr* callExpr) {
  Serial.print("{\"type\":\"callExpr\",\"caller\":");
  toString(callExpr->caller);
  Serial.print(",\"args\":[");
  for (size_t i = 0; i < callExpr->args.size(); i++) {
    toString(callExpr->args[i]);
    Serial.print(",");
  }
  Serial.print("]}");
}

void Parser::printAST(const Parser::Program* program) {
  Serial.print("{\"type\":\"program\",\"body\":[");

  size_t i = 0;
  while (true) {
    if (i < estimatedProgramStatements && program->body[i] != nullptr) {
      toString(program->body[i]);
      Serial.println(",");
      i++;
    } else {
      break;
    }
  }
  Serial.print("]}");
}

void Parser::toString(const Parser::Stmt* stmt) {
  switch (stmt->kind) {
    case Parser::NodeType::BinaryExpr:
      toStringBinaryExpr(static_cast<const Parser::BinaryExpr*>(stmt));
      break;
    case Parser::NodeType::LogicalExpr:
      toStringLogicalExpr(static_cast<const Parser::LogicalExpr*>(stmt));
      break;
    case Parser::NodeType::Identifier:
      toStringIdentifier(static_cast<const Parser::Identifier*>(stmt));
      break;
    case Parser::NodeType::NumericLiteral:
      toStringNumericLiteral(static_cast<const Parser::NumericLiteral*>(stmt));
      break;
    case Parser::NodeType::VarDeclaration:
      toStringVarDecl(static_cast<const Parser::VarDeclaration*>(stmt));
      break;
    case Parser::NodeType::IfStmt:
      toStringIfStmt(static_cast<const Parser::IfStmt*>(stmt));
      break;
    case Parser::NodeType::WhileStmt:
      toStringWhileStmt(static_cast<const Parser::WhileStmt*>(stmt));
      break;
    case Parser::NodeType::BreakStmt:
      toStringBreakStmt(static_cast<const Parser::BreakStmt*>(stmt));
      break;
    case Parser::NodeType::BlockStmt:
      toStringBlockStmt(static_cast<const Parser::BlockStmt*>(stmt));
      break;
    case Parser::NodeType::AssignmentExpr:
      toStringAssignmentExpr(static_cast<const Parser::AssignmentExpr*>(stmt));
      break;
    case Parser::NodeType::CallExpr:
      toStringCallExpr(static_cast<const Parser::CallExpr*>(stmt));
      break;
    case Parser::NodeType::Program:
      ErrorHandler::restart("Found Program in Program, don't know what to do with it");
      break;
  }
}