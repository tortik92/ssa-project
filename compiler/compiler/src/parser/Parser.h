#pragma once

#include "lexer/Lexer.h"
#include "GlobalFunctions.h"

class Parser {
public:
  Parser(Lexer* lexer) {
    this->lexer = lexer;
  }

  enum class NodeType {
    Undefined,
    // statements
    Program,
    VarDeclaration,
    // expressions
    AssignmentExpr,
    CallExpr,
    // literals
    NumericLiteral,
    Identifier,
    BinaryExpr,
  };

  typedef struct Stmt {
    NodeType kind;

    Stmt() {
      kind = NodeType::Undefined;
    }
    Stmt(NodeType _kind)
      : kind(_kind) {}
  } Stmt;

  typedef struct Program : Stmt {
    Stmt* body[maxProgramStatements];

    Program()
      : Stmt(NodeType::Program) {
      for (size_t i = 0; i < maxProgramStatements; i++) {
        body[i] = nullptr;
      }
    }
  } Program;


  typedef struct Expr : Stmt {
    Expr() {
      kind = NodeType::Undefined;
    }
    Expr(NodeType _nodeType)
      : Stmt(_nodeType) {}
  } Expr;

  typedef struct VarDeclaration : Stmt {
    bool constant;
    const char* ident;
    Expr* value;

    VarDeclaration()
      : Stmt(Parser::NodeType::VarDeclaration) {}
  } VarDeclaration;


  typedef struct AssignmentExpr : Expr {
    Expr* assignee;
    Expr* value;

    AssignmentExpr()
      : Expr(NodeType::AssignmentExpr) {}
  } AssignmentExpr;

  typedef struct CallExpr : Expr {
    Expr* caller;
    Expr* args[maxFunctionArgs];

    CallExpr()
      : Expr(NodeType::CallExpr) {
        for(size_t i = 0; i < maxFunctionArgs; i++) {
          args[i] = nullptr;
        }
      }
  } CallExpr;

  typedef struct BinaryExpr : Expr {
    Expr* left;
    Expr* right;
    char op;

    BinaryExpr()
      : Expr(NodeType::BinaryExpr) {}
  } BinaryExpr;

  typedef struct Identifier : Expr {
    const char* symbol;

    Identifier()
      : Expr(NodeType::Identifier) {}
  } Identifier;

  typedef struct NumericLiteral : Expr {
    int num;

    NumericLiteral()
      : Expr(NodeType::NumericLiteral) {}
  } NumericLiteral;


  Program* produceAST(char* code, size_t len);
private:
  Program program;
  Lexer* lexer;

  Lexer::Token* tokensPtr;
  size_t currentStmtIndex;

  BinaryExpr binaryExprPool[poolSize];
  Identifier identifierPool[poolSize];
  NumericLiteral numericLiteralPool[poolSize];
  VarDeclaration varDeclarationPool[poolSize];
  AssignmentExpr assignmentExprPool[poolSize];
  CallExpr callExprPool[poolSize];

  size_t binaryExprCount = 0;
  size_t identifierCount = 0;
  size_t numericLiteralCount = 0;
  size_t varDeclarationCount = 0;
  size_t assignentExprCount = 0;
  size_t callExprCount = 0;

  BinaryExpr* newBinaryExpr();
  Identifier* newIdentifier();
  NumericLiteral* newNumericLiteral();
  VarDeclaration* newVarDeclaration();
  AssignmentExpr* newAssignmentExpr();
  CallExpr* newCallExpr();

  void cleanup();

  Stmt* parseStmt();
  Expr* parseExpr();
  VarDeclaration* parseVarDeclaration();
  Expr* parseAssignmentExpr();
  Expr* parseAdditiveExpr();
  Expr* parseMultiplicativeExpr();
  Expr* parseCallMemberExpr();
  Expr* parseCallExpr(Expr* caller);
  void parseArgs(CallExpr* callExpr);
  void parseArgsList(CallExpr* callExpr);
  Expr* parsePrimaryExpr();

  bool endOfFile();
  void push(Stmt* stmt);

  Lexer::Token* at();
  Lexer::Token* eat();
  Lexer::Token* expect(Lexer::TokenType type, const char* err);
};