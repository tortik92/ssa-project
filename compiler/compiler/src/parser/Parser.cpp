#include "Parser.h"

Parser::Program* Parser::produceAST(char* code, size_t len) {
  tokens = lexer->tokenize(code, len);

  while (!endOfFile()) {
    push(parseStmt());
  }

  program.body.shrink_to_fit();

  return &program;
}

std::unique_ptr<Parser::Stmt> Parser::parseStmt() {
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

std::unique_ptr<Parser::VarDeclaration> Parser::parseVarDeclaration() {
  Serial.println("parseVarDeclaration");
  const bool isConstant = eat().type == Lexer::TokenType::Const;
  Lexer::Token varName = expect(Lexer::TokenType::Identifier, "identifier after let/const");

  std::unique_ptr<VarDeclaration> varDecl = std::make_unique<VarDeclaration>();
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

std::unique_ptr<Parser::IfStmt> Parser::parseIfStmt() {
  Serial.println("parseIfStmt");
  eat();  // consume 'if'
  std::unique_ptr<IfStmt> ifStmt = std::make_unique<IfStmt>();

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

std::unique_ptr<Parser::WhileStmt> Parser::parseWhileStmt() {
  Serial.println("parseWhileStmt");
  eat();  // consume 'while'
  std::unique_ptr<WhileStmt> whileStmt = std::make_unique<WhileStmt>();

  expect(Lexer::TokenType::OpenParen, "(");
  whileStmt->test = parseExpr();
  expect(Lexer::TokenType::CloseParen, ")");

  whileStmt->body = parseBlockStmt();

  return whileStmt;
}

std::unique_ptr<Parser::BreakStmt> Parser::parseBreakStmt() {
  Serial.println("parseBreakStmt");
  eat();  // consume 'break'
  std::unique_ptr<BreakStmt> breakStmt = std::make_unique<BreakStmt>();
  expect(Lexer::TokenType::Semicolon, ";");
  return breakStmt;
}

std::unique_ptr<Parser::BlockStmt> Parser::parseBlockStmt() {
  Serial.println("parseBlockStmt");
  expect(Lexer::TokenType::OpenBrace, "{");
  std::unique_ptr<BlockStmt> blockStmt = std::make_unique<BlockStmt>();
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

std::unique_ptr<Parser::Expr> Parser::parseExpr() {
  Serial.println("parseExpr");
  return parseLogicalExpr();
}

std::unique_ptr<Parser::Expr> Parser::parseLogicalExpr() {
  Serial.println("parseLogicalExpr");
  std::unique_ptr<Expr> left = parseRelationalExpr();

  while (strcmp(at().value, "and") == 0 || strcmp(at().value, "or") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<LogicalExpr> logicalExpr = std::make_unique<LogicalExpr>();
    logicalExpr->left = std::move(left);
    logicalExpr->right = parseRelationalExpr();
    logicalExpr->op = op.value;
    op.value = nullptr;

    return logicalExpr;
  }

  Serial.println("return parseLogicalExpr");

  return left;
}

std::unique_ptr<Parser::Expr> Parser::parseRelationalExpr() {
  Serial.println("parseRelationalExpr");
  std::unique_ptr<Expr> left = parseAssignmentExpr();

  while (strcmp(at().value, "<") == 0 || strcmp(at().value, "<=") == 0 || strcmp(at().value, ">") == 0 || strcmp(at().value, ">=") == 0 || strcmp(at().value, "==") == 0 || strcmp(at().value, "!=") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<BinaryExpr> relationalExpr = std::make_unique<BinaryExpr>();
    relationalExpr->left = std::move(left);
    relationalExpr->right = parseAssignmentExpr();
    relationalExpr->op = op.value;
    op.value = nullptr;

    return relationalExpr;
  }

  Serial.println("return parseRelationalExpr");

  return left;
}

std::unique_ptr<Parser::Expr> Parser::parseAssignmentExpr() {
  Serial.println("parseAssignmentExpr");
  std::unique_ptr<Expr> left = parseObjectExpr();

  if (at().type == Lexer::TokenType::Equals) {
    eat();
    if (left->kind != NodeType::Identifier) {
      ErrorHandler::restart("Expected variable name for assignment");
    }
    std::unique_ptr<AssignmentExpr> assignmentExpr = std::make_unique<AssignmentExpr>();
    assignmentExpr->assignee = std::move(left);
    assignmentExpr->value = parseAssignmentExpr();

    expect(Lexer::TokenType::Semicolon, ";");

    return assignmentExpr;
  }

  Serial.println("return parseAssignmentExpr");

  return left;
}

std::unique_ptr<Parser::Expr> Parser::parseObjectExpr() {
  Serial.println("parseObjectExpr");

  if (at().type != Lexer::TokenType::OpenBrace) {
    return parseAdditiveExpr();
  }

  eat();
  std::unique_ptr<ObjectLiteral> objectLiteral = std::make_unique<ObjectLiteral>();

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
      std::unique_ptr<Expr> value = parseExpr();
      objectLiteral->properties[keyToken.value] = std::move(value);

      if (at().type != Lexer::TokenType::CloseBrace) {
        expect(Lexer::TokenType::Comma, ",");
      }
    }
  }

  expect(Lexer::TokenType::CloseBrace, "}");

  return objectLiteral;
}

std::unique_ptr<Parser::Expr> Parser::parseAdditiveExpr() {
  Serial.println("parseAdditiveExpr");
  std::unique_ptr<Expr> leftMost = parseMultiplicativeExpr();

  while (strcmp(at().value, "+") == 0 || strcmp(at().value, "-") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<BinaryExpr> binaryExpr = std::make_unique<BinaryExpr>();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = std::move(leftMost);
    binaryExpr->right = parseMultiplicativeExpr();
    binaryExpr->op = op.value;
    op.value = nullptr;

    leftMost = std::move(binaryExpr);
  }

  Serial.println("return parseAdditiveExpr");

  return leftMost;
}

std::unique_ptr<Parser::Expr> Parser::parseMultiplicativeExpr() {
  Serial.println("parseMultiplicativeExpr");
  std::unique_ptr<Expr> leftMost = parseCallMemberExpr();

  while (strcmp(at().value, "*") == 0 || strcmp(at().value, "/") == 0 || strcmp(at().value, "%") == 0) {
    Lexer::Token op = eat();

    std::unique_ptr<BinaryExpr> binaryExpr = std::make_unique<BinaryExpr>();
    binaryExpr->kind = NodeType::BinaryExpr;
    binaryExpr->left = std::move(leftMost);
    binaryExpr->right = parseCallMemberExpr();
    binaryExpr->op = op.value;
    op.value = nullptr;

    leftMost = std::move(binaryExpr);
  }

  Serial.println("return parseMultiplicativeExpr");

  return leftMost;
}

std::unique_ptr<Parser::Expr> Parser::parseCallMemberExpr() {
  Serial.println("parseCallMemberExpr");
  std::unique_ptr<Expr> member = parseMemberExpr();

  if (at().type == Lexer::TokenType::OpenParen) {
    Serial.println("Found OpenParen");
    return parseCallExpr(std::move(member));
  }

  Serial.println("return parseCallMemberExpr");

  return member;
}

std::unique_ptr<Parser::CallExpr> Parser::parseCallExpr(std::unique_ptr<Expr>&& caller) {
  Serial.println("parseCallExpr");
  std::unique_ptr<CallExpr> callExpr = std::make_unique<CallExpr>();
  callExpr->caller = std::move(caller);

  parseArgs(callExpr);
  expect(Lexer::TokenType::Semicolon, ";");

  return callExpr;
}

void Parser::parseArgs(std::unique_ptr<CallExpr>& callExpr) {
  Serial.println("parseArgs");
  expect(Lexer::TokenType::OpenParen, "(");
  if (strcmp(at().value, ")") != 0) {
    parseArgsList(callExpr);
  }
  expect(Lexer::TokenType::CloseParen, ")");
}

void Parser::parseArgsList(std::unique_ptr<CallExpr>& callExpr) {
  Serial.println("parseArgsList");
  
  std::unique_ptr<Expr> firstParam = parseExpr();
  callExpr->args.push_back(std::move(firstParam));

  while (at().type == Lexer::TokenType::Comma) {
    eat();
    callExpr->args.push_back(parseExpr());
  }
}

std::unique_ptr<Parser::Expr> Parser::parseMemberExpr() {
  std::unique_ptr<Expr> object = parsePrimaryExpr();

  while (at().type == Lexer::TokenType::Dot || at().type == Lexer::TokenType::OpenBracket) {
    const Lexer::Token op = eat();
    std::unique_ptr<Expr> property;
    bool computed;

    if (op.type == Lexer::TokenType::Dot) {
      computed = false;
      property = parsePrimaryExpr();

      if (property->kind != NodeType::Identifier) {
        ErrorHandler::restart("Cannot use dot operator without right hand side being an identifier");
      }

      Serial.print("Found dot, property: ");
      Serial.println(static_cast<Parser::Identifier*>(property.get())->symbol);
    } else {
      computed = true;
      property = parseExpr();
      expect(Lexer::TokenType::CloseBracket, "]");
    }

    std::unique_ptr<MemberExpr> memberExpr = std::make_unique<MemberExpr>();
    memberExpr->object = std::move(object);
    memberExpr->property = std::move(property);
    memberExpr->computed = computed;

    if (memberExpr->property->kind == NodeType::Identifier) {
      Serial.print("MemberExpr property identifier: ");
      Serial.println(static_cast<Identifier*>(memberExpr->property.get())->symbol);
    }

    object = std::move(memberExpr);
  }

  return object;
}

std::unique_ptr<Parser::Expr> Parser::parsePrimaryExpr() {
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
        std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();

        Lexer::Token identToken = eat();

        identifier->symbol = identToken.value;
        identToken.value = nullptr;

        Serial.print("Found identifier ");
        Serial.println(identifier->symbol);

        return identifier;
      }
    case Lexer::TokenType::Number:
      {
        std::unique_ptr<NumericLiteral> number = std::make_unique<NumericLiteral>();

        number->num = strtof(eat().value, nullptr);

        Serial.print("Found number ");
        Serial.println(number->num);

        return number;
      }
    case Lexer::TokenType::StringLiteral:
      {
        std::unique_ptr<StringLiteral> str = std::make_unique<StringLiteral>(eat().value);

        Serial.print("Found string ");
        Serial.println(str->value);

        return str;
      }
    case Lexer::TokenType::OpenParen:
      {
        eat();  // consume '('
        std::unique_ptr<Expr> val = parseExpr();

        expect(Lexer::TokenType::CloseParen, ")");
        return val;
      }
    default:
      ErrorHandler::restart("Unexpected token \"", at().value, "\" found!");
      return std::make_unique<Parser::Identifier>();
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

void Parser::push(std::unique_ptr<Stmt>&& stmt) {
  program.body.push_back(std::move(stmt));

  if (currentStmtIndex < estimatedProgramStatements) {
    currentStmtIndex++;
  } else {
    ErrorHandler::restart("Too many statements in program");
  }
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

void Parser::toStringStringLiteral(const Parser::StringLiteral* str) {
  Serial.print("{\"type\":\"stringLiteral\",\"value\":\"");
  Serial.print(str->value);
  Serial.print("\",\"raw\":\"");
  Serial.print(str->raw);
  Serial.print("\"}");
}

void Parser::toStringBinaryExpr(const Parser::BinaryExpr* binaryExpr) {
  Serial.print("{\"type\":\"binaryExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt*>(binaryExpr->left.get()));
  Serial.print(",\"operator\":\"");
  Serial.print(binaryExpr->op);
  Serial.print("\",");
  Serial.print("\"right\":");
  toString(static_cast<const Parser::Stmt*>(binaryExpr->right.get()));
  Serial.print("}");
}

void Parser::toStringLogicalExpr(const Parser::LogicalExpr* logicalExpr) {
  Serial.print("{\"type\":\"logicalExpr\",");
  Serial.print("\"left\":");
  toString(static_cast<const Parser::Stmt*>(logicalExpr->left.get()));
  Serial.print(",\"operator\":\"");
  Serial.print(logicalExpr->op);
  Serial.print("\",\"right\":");
  toString(static_cast<const Parser::Stmt*>(logicalExpr->right.get()));
  Serial.print("}");
}

void Parser::toStringVarDecl(const Parser::VarDeclaration* varDecl) {
  Serial.print("{\"type\":\"varDecl\",");
  Serial.print("\"isConstant\":");
  Serial.print(varDecl->constant ? "\"true\"," : "\"false\",");
  Serial.print("\"identifier\":\"");
  Serial.print(varDecl->ident);
  Serial.print("\",\"value\":");
  if (varDecl->value != nullptr) toString(varDecl->value.get());
  Serial.print("}");
}

void Parser::toStringBlockStmt(const Parser::BlockStmt* blockStmt) {
  Serial.print("{\"type\":\"blockStmt\",\"body\":[");
  for (size_t i = 0; i < blockStmt->body.size(); i++) {
    toString(blockStmt->body[i].get());
    if (i + 1 < blockStmt->body.size()) {
      Serial.print(",");
    }
  }
  Serial.print("]}");
}

void Parser::toStringIfStmt(const Parser::IfStmt* ifStmt) {
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

void Parser::toStringWhileStmt(const Parser::WhileStmt* whileStmt) {
  Serial.print("{\"type\":\"whileStmt\",");
  Serial.print("\"test\":");
  toString(whileStmt->test.get());
  Serial.print(",\"body\":");
  toStringBlockStmt(whileStmt->body.get());
  Serial.print("}");
}

void Parser::toStringBreakStmt(const Parser::BreakStmt* breakStmt) {
  Serial.print("{\"type\":\"breakStmt\"}");
}

void Parser::toStringAssignmentExpr(const Parser::AssignmentExpr* assignmentExpr) {
  Serial.print("{\"type\":\"assignmentExpr\",\"assignee\":");
  toString(assignmentExpr->assignee.get());
  Serial.print(",\"value\":");
  toString(assignmentExpr->value.get());
  Serial.print("}");
}

void Parser::toStringObjectLiteral(const Parser::ObjectLiteral* objectLiteral) {
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

void Parser::toStringCallExpr(const Parser::CallExpr* callExpr) {
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

void Parser::toStringMemberExpr(const Parser::MemberExpr* memberExpr) {
  Serial.print("{\"type\":\"memberExpr\",");
  Serial.print("\"computed\":");
  Serial.print(memberExpr->computed ? "\"true\"," : "\"false\",");
  Serial.print("\"object\":");
  toString(memberExpr->object.get());
  Serial.print(",\"property\":");
  toString(memberExpr->property.get());
  Serial.print("}");
}

void Parser::printAST(const Parser::Program* program) {
  Serial.print("{\"type\":\"program\",\"body\":[");

  for (size_t i = 0; i < program->body.size(); i++) {
    toString(program->body[i].get());
    if (i + 1 < program->body.size()) {
      Serial.println(",");
    }
  }

  Serial.println("]}");
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
    case Parser::NodeType::StringLiteral:
      toStringStringLiteral(static_cast<const Parser::StringLiteral*>(stmt));
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
    case Parser::NodeType::ObjectLiteral:
      toStringObjectLiteral(static_cast<const Parser::ObjectLiteral*>(stmt));
      break;
    case Parser::NodeType::CallExpr:
      toStringCallExpr(static_cast<const Parser::CallExpr*>(stmt));
      break;
    case Parser::NodeType::MemberExpr:
      toStringMemberExpr(static_cast<const Parser::MemberExpr*>(stmt));
      break;
    case Parser::NodeType::Program:
      ErrorHandler::restart("Found Program in Program, don't know what to do with it");
      break;
  }
}