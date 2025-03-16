#include "Parser.h"

AstNodes::Program* Parser::produceAST(char* code, size_t len) {
  tokens = lexer->tokenize(code, len);

  while (!endOfFile()) {
    push(parseStmt());
  }

  program.body.shrink_to_fit();

  return &program;
}

std::unique_ptr<AstNodes::Stmt> Parser::parseStmt() {
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

std::unique_ptr<AstNodes::VarDeclaration> Parser::parseVarDeclaration() {
  Serial.println("parseVarDeclaration");
  const bool isConstant = eat().type == Lexer::TokenType::Const;
  Lexer::Token varName = expect(Lexer::TokenType::Identifier, "identifier after let/const");

  std::unique_ptr<AstNodes::VarDeclaration> varDecl = std::make_unique<AstNodes::VarDeclaration>();
  varDecl->constant = isConstant;
  varDecl->ident = varName.value;
  varName.value = nullptr;

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

std::unique_ptr<AstNodes::IfStmt> Parser::parseIfStmt() {
  Serial.println("parseIfStmt");
  eat();  // consume 'if'
  std::unique_ptr<AstNodes::IfStmt> ifStmt = std::make_unique<AstNodes::IfStmt>();

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

std::unique_ptr<AstNodes::WhileStmt> Parser::parseWhileStmt() {
  Serial.println("parseWhileStmt");
  eat();  // consume 'while'
  std::unique_ptr<AstNodes::WhileStmt> whileStmt = std::make_unique<AstNodes::WhileStmt>();

  expect(Lexer::TokenType::OpenParen, "(");
  whileStmt->test = parseExpr();
  expect(Lexer::TokenType::CloseParen, ")");

  whileStmt->body = parseBlockStmt();

  return whileStmt;
}

std::unique_ptr<AstNodes::BreakStmt> Parser::parseBreakStmt() {
  Serial.println("parseBreakStmt");
  eat();  // consume 'break'
  std::unique_ptr<AstNodes::BreakStmt> breakStmt = std::make_unique<AstNodes::BreakStmt>();
  expect(Lexer::TokenType::Semicolon, ";");
  return breakStmt;
}

std::unique_ptr<AstNodes::BlockStmt> Parser::parseBlockStmt() {
  Serial.println("parseBlockStmt");
  expect(Lexer::TokenType::OpenBrace, "{");
  std::unique_ptr<AstNodes::BlockStmt> blockStmt = std::make_unique<AstNodes::BlockStmt>();
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

std::unique_ptr<AstNodes::Expr> Parser::parseExpr() {
  Serial.println("parseExpr");
  return parseLogicalExpr();
}

std::unique_ptr<AstNodes::Expr> Parser::parseLogicalExpr() {
  Serial.println("parseLogicalExpr");
  std::unique_ptr<AstNodes::Expr> left = parseRelationalExpr();

  while (at().type == Lexer::TokenType::LogicalOperator) {  // and, or
    Lexer::Token op = eat();

    std::unique_ptr<AstNodes::LogicalExpr> logicalExpr = std::make_unique<AstNodes::LogicalExpr>();
    logicalExpr->left = std::move(left);
    logicalExpr->right = parseRelationalExpr();
    logicalExpr->op = op.value;
    op.value = nullptr;

    left = std::move(logicalExpr);
  }

  Serial.println("return parseLogicalExpr");

  return left;
}

std::unique_ptr<AstNodes::Expr> Parser::parseRelationalExpr() {
  Serial.println("parseRelationalExpr");
  std::unique_ptr<AstNodes::Expr> left = parseAssignmentExpr();

  while (strcmp(at().value, "<") == 0 || strcmp(at().value, "<=") == 0 || strcmp(at().value, ">") == 0 || strcmp(at().value, ">=") == 0 || strcmp(at().value, "==") == 0 || strcmp(at().value, "!=") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<AstNodes::BinaryExpr> relationalExpr = std::make_unique<AstNodes::BinaryExpr>();
    relationalExpr->left = std::move(left);
    relationalExpr->right = parseRelationalExpr();
    relationalExpr->op = op.value;
    op.value = nullptr;

    return relationalExpr;
  }

  Serial.println("return parseRelationalExpr");

  return left;
}

std::unique_ptr<AstNodes::Expr> Parser::parseAssignmentExpr() {
  Serial.println("parseAssignmentExpr");
  std::unique_ptr<AstNodes::Expr> left = parseObjectExpr();

  if (at().type == Lexer::TokenType::Equals) {
    eat();
    if (left->kind != AstNodes::NodeType::Identifier) {
      ErrorHandler::restart("Expected variable name for assignment");
    }
    std::unique_ptr<AstNodes::AssignmentExpr> assignmentExpr = std::make_unique<AstNodes::AssignmentExpr>();
    assignmentExpr->assignee = std::move(left);
    assignmentExpr->value = parseAssignmentExpr();

    expect(Lexer::TokenType::Semicolon, ";");

    return assignmentExpr;
  }

  Serial.println("return parseAssignmentExpr");

  return left;
}

std::unique_ptr<AstNodes::Expr> Parser::parseObjectExpr() {
  Serial.println("parseObjectExpr");

  if (at().type != Lexer::TokenType::OpenBrace) {
    return parseAdditiveExpr();
  }

  eat();
  std::unique_ptr<AstNodes::ObjectLiteral> objectLiteral = std::make_unique<AstNodes::ObjectLiteral>();

  while (!endOfFile() && at().type != Lexer::TokenType::CloseBrace) {
    Lexer::Token keyToken = expect(Lexer::TokenType::Identifier, "identifier");

    // options for { key, [...]} and { key }
    if (at().type == Lexer::TokenType::Comma || at().type == Lexer::TokenType::CloseBrace) {
      if (at().type == Lexer::TokenType::Comma) {
        eat();
      }

      objectLiteral->properties[keyToken.value] = nullptr;
    } else {
      expect(Lexer::TokenType::Colon, ":");
      std::unique_ptr<AstNodes::Expr> value = parseExpr();
      objectLiteral->properties[keyToken.value] = std::move(value);

      if (at().type != Lexer::TokenType::CloseBrace) {
        expect(Lexer::TokenType::Comma, ",");
      }
    }
  }

  expect(Lexer::TokenType::CloseBrace, "}");

  return objectLiteral;
}

std::unique_ptr<AstNodes::Expr> Parser::parseAdditiveExpr() {
  Serial.println("parseAdditiveExpr");
  std::unique_ptr<AstNodes::Expr> leftMost = parseMultiplicativeExpr();

  while (strcmp(at().value, "+") == 0 || strcmp(at().value, "-") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<AstNodes::BinaryExpr> binaryExpr = std::make_unique<AstNodes::BinaryExpr>();
    binaryExpr->kind = AstNodes::NodeType::BinaryExpr;
    binaryExpr->left = std::move(leftMost);
    binaryExpr->right = parseMultiplicativeExpr();
    binaryExpr->op = op.value;
    op.value = nullptr;

    leftMost = std::move(binaryExpr);
  }

  Serial.println("return parseAdditiveExpr");

  return leftMost;
}

std::unique_ptr<AstNodes::Expr> Parser::parseMultiplicativeExpr() {
  Serial.println("parseMultiplicativeExpr");
  std::unique_ptr<AstNodes::Expr> leftMost = parseCallMemberExpr();

  while (strcmp(at().value, "*") == 0 || strcmp(at().value, "/") == 0 || strcmp(at().value, "%") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<AstNodes::BinaryExpr> binaryExpr = std::make_unique<AstNodes::BinaryExpr>();
    binaryExpr->kind = AstNodes::NodeType::BinaryExpr;
    binaryExpr->left = std::move(leftMost);
    binaryExpr->right = parseCallMemberExpr();
    binaryExpr->op = op.value;
    op.value = nullptr;

    leftMost = std::move(binaryExpr);
  }

  Serial.println("return parseMultiplicativeExpr");

  return leftMost;
}

std::unique_ptr<AstNodes::Expr> Parser::parseCallMemberExpr() {
  Serial.println("parseCallMemberExpr");
  std::unique_ptr<AstNodes::Expr> member = parseMemberExpr();

  if (at().type == Lexer::TokenType::OpenParen) {
    Serial.println("Found OpenParen");
    return parseCallExpr(std::move(member));
  }

  Serial.println("return parseCallMemberExpr");

  return member;
}

std::unique_ptr<AstNodes::CallExpr> Parser::parseCallExpr(std::unique_ptr<AstNodes::Expr>&& caller) {
  Serial.println("parseCallExpr");
  std::unique_ptr<AstNodes::CallExpr> callExpr = std::make_unique<AstNodes::CallExpr>();
  callExpr->caller = std::move(caller);

  parseArgs(callExpr);
  expect(Lexer::TokenType::Semicolon, ";");

  return callExpr;
}

void Parser::parseArgs(std::unique_ptr<AstNodes::CallExpr>& callExpr) {
  Serial.println("parseArgs");
  expect(Lexer::TokenType::OpenParen, "(");
  if (strcmp(at().value, ")") != 0) {
    parseArgsList(callExpr);
  }
  expect(Lexer::TokenType::CloseParen, ")");
}

void Parser::parseArgsList(std::unique_ptr<AstNodes::CallExpr>& callExpr) {
  Serial.println("parseArgsList");
  
  std::unique_ptr<AstNodes::Expr> firstParam = parseExpr();
  callExpr->args.push_back(std::move(firstParam));

  while (at().type == Lexer::TokenType::Comma) {
    eat();
    callExpr->args.push_back(parseExpr());
  }
}

std::unique_ptr<AstNodes::Expr> Parser::parseMemberExpr() {
  std::unique_ptr<AstNodes::Expr> object = parsePrimaryExpr();

  while (at().type == Lexer::TokenType::Dot || at().type == Lexer::TokenType::OpenBracket) {
    const Lexer::Token op = eat();
    std::unique_ptr<AstNodes::Expr> property;
    bool computed;

    if (op.type == Lexer::TokenType::Dot) {
      computed = false;
      property = parsePrimaryExpr();

      if (property->kind != AstNodes::NodeType::Identifier) {
        ErrorHandler::restart("Cannot use dot operator without right hand side being an identifier");
      }

      Serial.print("Found dot, property: ");
      Serial.println(static_cast<AstNodes::Identifier*>(property.get())->symbol);
    } else {
      computed = true;
      property = parseExpr();
      expect(Lexer::TokenType::CloseBracket, "]");
    }

    std::unique_ptr<AstNodes::MemberExpr> memberExpr = std::make_unique<AstNodes::MemberExpr>();
    memberExpr->object = std::move(object);
    memberExpr->property = std::move(property);
    memberExpr->computed = computed;

    if (memberExpr->property->kind == AstNodes::NodeType::Identifier) {
      Serial.print("MemberExpr property identifier: ");
      Serial.println(static_cast<AstNodes::Identifier*>(memberExpr->property.get())->symbol);
    }

    object = std::move(memberExpr);
  }

  return object;
}

std::unique_ptr<AstNodes::Expr> Parser::parsePrimaryExpr() {
  Serial.println("parsePrimaryExpr");

  switch (at().type) {
    case Lexer::TokenType::Identifier:
      {
        std::unique_ptr<AstNodes::Identifier> identifier = std::make_unique<AstNodes::Identifier>();

        Lexer::Token identToken = eat();

        identifier->symbol = identToken.value;
        identToken.value = nullptr;

        Serial.print("Found identifier ");
        Serial.println(identifier->symbol);

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        std::unique_ptr<AstNodes::NumericLiteral> number = std::make_unique<AstNodes::NumericLiteral>();

        number->num = strtof(eat().value, nullptr);

        Serial.print("Found number ");
        Serial.println(number->num);

        return number;
      }
    case Lexer::TokenType::StringLiteral:
      {
        std::unique_ptr<AstNodes::StringLiteral> str = std::make_unique<AstNodes::StringLiteral>(eat().value);

        Serial.print("Found string ");
        Serial.println(str->value);

        return str;
      }
    case Lexer::TokenType::OpenParen:
      {
        eat();  // consume '('
        std::unique_ptr<AstNodes::Expr> val = parseExpr();

        expect(Lexer::TokenType::CloseParen, ")");
        return val;
      }
    default:
      ErrorHandler::restart("Unexpected token \"", at().value, "\" found!");
      return std::make_unique<AstNodes::Identifier>();
  }
}

Lexer::Token& Parser::at() {
  if (tokens.empty()) {
    ErrorHandler::restart("Trying to access empty tokens queue");
  }
  return tokens.front();
}

Lexer::Token Parser::eat() {
  Lexer::Token returnValue = std::move(tokens.front());  // queue emptiness is already checked in !endOfFile()
  tokens.pop();
  return returnValue;
}

Lexer::Token Parser::expect(Lexer::TokenType type, const char* expectedVal) {
  Lexer::Token prev = eat();
  if (prev.type != type) {
    ErrorHandler::missingToken(expectedVal);
  }
  Serial.print("Found expected '");
  Serial.print(prev.value);
  Serial.println("'");
  return prev;
}

bool Parser::endOfFile() {
  return tokens.front().type == Lexer::TokenType::EndOfFile;
}

void Parser::push(std::unique_ptr<AstNodes::Stmt>&& stmt) {
  program.body.push_back(std::move(stmt));

  if (currentStmtIndex < estimatedProgramStatements) {
    currentStmtIndex++;
  } else {
    ErrorHandler::restart("Too many statements in program");
  }
}

// toString
void Parser::toStringNumericLiteral(const AstNodes::NumericLiteral* numLit) {
  Serial.print("{\"type\":\"numericLiteral\",\"value\":\"");
  Serial.print(numLit->num);
  Serial.print("\"}");
}

void Parser::toStringIdentifier(const AstNodes::Identifier* ident) {
  Serial.print("{\"type\":\"identifier\",\"symbol\":\"");
  Serial.print(ident->symbol);
  Serial.print("\"}");
}

void Parser::toStringStringLiteral(const AstNodes::StringLiteral* str) {
  Serial.print("{\"type\":\"stringLiteral\",\"value\":\"");
  Serial.print(str->value);
  Serial.print("\",\"raw\":\"");
  Serial.print(str->raw);
  Serial.print("\"}");
}

void Parser::toStringBinaryExpr(const AstNodes::BinaryExpr* binaryExpr) {
  Serial.print("{\"type\":\"binaryExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const AstNodes::Stmt*>(binaryExpr->left.get()));
  Serial.print(",\"operator\":\"");
  Serial.print(binaryExpr->op);
  Serial.print("\",");
  Serial.print("\"right\":");
  toString(static_cast<const AstNodes::Stmt*>(binaryExpr->right.get()));
  Serial.print("}");
}

void Parser::toStringLogicalExpr(const AstNodes::LogicalExpr* logicalExpr) {
  Serial.print("{\"type\":\"logicalExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const AstNodes::Stmt*>(logicalExpr->left.get()));
  Serial.print(",\"operator\":\"");
  Serial.print(logicalExpr->op);
  Serial.print("\",\"right\":");
  toString(static_cast<const AstNodes::Stmt*>(logicalExpr->right.get()));
  Serial.print("}");
}

void Parser::toStringVarDecl(const AstNodes::VarDeclaration* varDecl) {
  Serial.print("{\"type\":\"varDecl\",");
  Serial.print("\"isConstant\":");
  Serial.print(varDecl->constant ? "\"true\"," : "\"false\",");
  Serial.print("\"identifier\":\"");
  Serial.print(varDecl->ident);
  Serial.print("\",\"value\":");
  if (varDecl->value != nullptr) toString(varDecl->value.get());
  Serial.print("}");
}

void Parser::toStringBlockStmt(const AstNodes::BlockStmt* blockStmt) {
  Serial.print("{\"type\":\"blockStmt\",\"body\":[");
  for (size_t i = 0; i < blockStmt->body.size(); i++) {
    toString(blockStmt->body[i].get());
    if (i + 1 < blockStmt->body.size()) {
      Serial.print(",");
    }
  }
  Serial.print("]}");
}

void Parser::toStringIfStmt(const AstNodes::IfStmt* ifStmt) {
  Serial.print("{\"type\":\"ifStmt\",");
  Serial.print("\"test\":");
  toString(ifStmt->test.get());
  Serial.print(",\"consequent\":");
  toStringBlockStmt(ifStmt->consequent.get());
  Serial.print(",\"alternate\":");
  if (ifStmt->alternate != nullptr) {
    toStringBlockStmt(ifStmt->alternate.get());
  } else {
    Serial.print("null");
  }
  Serial.print("}");
}

void Parser::toStringWhileStmt(const AstNodes::WhileStmt* whileStmt) {
  Serial.print("{\"type\":\"whileStmt\",");
  Serial.print("\"test\":");
  toString(whileStmt->test.get());
  Serial.print(",\"body\":");
  toStringBlockStmt(whileStmt->body.get());
  Serial.print("}");
}

void Parser::toStringBreakStmt(const AstNodes::BreakStmt* breakStmt) {
  Serial.print("{\"type\":\"breakStmt\"}");
}

void Parser::toStringAssignmentExpr(const AstNodes::AssignmentExpr* assignmentExpr) {
  Serial.print("{\"type\":\"assignmentExpr\",\"assignee\":");
  toString(assignmentExpr->assignee.get());
  Serial.print(",\"value\":");
  toString(assignmentExpr->value.get());
  Serial.print("}");
}

void Parser::toStringObjectLiteral(const AstNodes::ObjectLiteral* objectLiteral) {
  Serial.print("{\"type\":\"objectLiteral\",\"properties\":{");
  for (const auto& [key, value] : objectLiteral->properties) {
    Serial.print("\"");
    Serial.print(key);
    Serial.print("\":");
    toString(value.get());

    if (key != objectLiteral->properties.rbegin()->first) {
      Serial.print(",");
    }
  }
  Serial.print("}}");
}

void Parser::toStringCallExpr(const AstNodes::CallExpr* callExpr) {
  Serial.print("{\"type\":\"callExpr\",\"caller\":");
  toString(callExpr->caller.get());
  Serial.print(",\"args\":[");
  for (size_t i = 0; i < callExpr->args.size(); i++) {
    toString(callExpr->args[i].get());
    if (i + 1 < callExpr->args.size()) {
      Serial.print(",");
    }
  }
  Serial.print("]}");
}

void Parser::toStringMemberExpr(const AstNodes::MemberExpr* memberExpr) {
  Serial.print("{\"type\":\"memberExpr\",");
  Serial.print("\"computed\":");
  Serial.print(memberExpr->computed ? "\"true\"," : "\"false\",");
  Serial.print("\"object\":");
  toString(memberExpr->object.get());
  Serial.print(",\"property\":");
  toString(memberExpr->property.get());
  Serial.print("}");
}

void Parser::printAST(const AstNodes::Program* program) {
  Serial.print("{\"type\":\"program\",\"body\":[");

  for (size_t i = 0; i < program->body.size(); i++) {
    toString(program->body[i].get());
    if (i + 1 < program->body.size()) {
      Serial.println(",");
    }
  }

  Serial.println("]}");
}

void Parser::toString(const AstNodes::Stmt* stmt) {
  switch (stmt->kind) {
    case AstNodes::NodeType::BinaryExpr:
      toStringBinaryExpr(static_cast<const AstNodes::BinaryExpr*>(stmt));
      break;
    case AstNodes::NodeType::LogicalExpr:
      toStringLogicalExpr(static_cast<const AstNodes::LogicalExpr*>(stmt));
      break;
    case AstNodes::NodeType::Identifier:
      toStringIdentifier(static_cast<const AstNodes::Identifier*>(stmt));
      break;
    case AstNodes::NodeType::NumericLiteral:
      toStringNumericLiteral(static_cast<const AstNodes::NumericLiteral*>(stmt));
      break;
    case AstNodes::NodeType::StringLiteral:
      toStringStringLiteral(static_cast<const AstNodes::StringLiteral*>(stmt));
      break;
    case AstNodes::NodeType::VarDeclaration:
      toStringVarDecl(static_cast<const AstNodes::VarDeclaration*>(stmt));
      break;
    case AstNodes::NodeType::IfStmt:
      toStringIfStmt(static_cast<const AstNodes::IfStmt*>(stmt));
      break;
    case AstNodes::NodeType::WhileStmt:
      toStringWhileStmt(static_cast<const AstNodes::WhileStmt*>(stmt));
      break;
    case AstNodes::NodeType::BreakStmt:
      toStringBreakStmt(static_cast<const AstNodes::BreakStmt*>(stmt));
      break;
    case AstNodes::NodeType::BlockStmt:
      toStringBlockStmt(static_cast<const AstNodes::BlockStmt*>(stmt));
      break;
    case AstNodes::NodeType::AssignmentExpr:
      toStringAssignmentExpr(static_cast<const AstNodes::AssignmentExpr*>(stmt));
      break;
    case AstNodes::NodeType::ObjectLiteral:
      toStringObjectLiteral(static_cast<const AstNodes::ObjectLiteral*>(stmt));
      break;
    case AstNodes::NodeType::CallExpr:
      toStringCallExpr(static_cast<const AstNodes::CallExpr*>(stmt));
      break;
    case AstNodes::NodeType::MemberExpr:
      toStringMemberExpr(static_cast<const AstNodes::MemberExpr*>(stmt));
      break;
    case AstNodes::NodeType::Program:
      ErrorHandler::restart("Found Program in Program, don't know what to do with it");
      break;
  }
}