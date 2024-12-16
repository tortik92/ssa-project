#pragma once

#include "lexer/Lexer.h"
#include "GlobalFunctions.h"

class Parser {
public:
  Parser(Lexer* lexer) {
    this->lexer = lexer;
  }

  ~Parser() {
    delete[] binaryExprPool;
    delete[] identifierPool;
    delete[] numericLiteralPool;
    delete[] varDeclarationPool;
    delete[] assignmentExprPool;
    delete[] callExprPool;
    delete[] logicalExprPool;
    delete[] blockStmtPool;
    delete[] ifStmtPool;
  }

  enum class NodeType {
    Undefined,
    // statements
    Program,
    VarDeclaration,
    IfStmt,
    WhileStmt,
    BreakStmt,
    BlockStmt,
    // expressions
    AssignmentExpr,
    CallExpr,
    // literals
    NumericLiteral,
    Identifier,
    BinaryExpr,
    LogicalExpr
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
      for (size_t i = 0; i < maxFunctionArgs; i++) {
        args[i] = nullptr;
      }
    }
  } CallExpr;

  typedef struct BinaryExpr : Expr {
    Expr* left;
    Expr* right;
    char* op;

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

  typedef struct LogicalExpr : Expr {
    Expr* left;
    Expr* right;
    char* op;

    LogicalExpr()
      : Expr(NodeType::LogicalExpr) {}
  } LogicalExpr;

  typedef struct BlockStmt : Stmt {
    Stmt* body[maxBlockStatements];

    BlockStmt()
      : Stmt(NodeType::BlockStmt) {}
  } BlockStmt;

  typedef struct IfStmt : Stmt {
    Expr* test;
    BlockStmt* consequent;
    BlockStmt* alternate;

    IfStmt()
      : Stmt(NodeType::IfStmt) {}
  } IfStmt;

  typedef struct WhileStmt : Stmt {
    Expr* test;
    BlockStmt* body;

    WhileStmt()
      : Stmt(NodeType::WhileStmt) {}
  } WhileStmt;

  typedef struct BreakStmt : Stmt {
    BreakStmt()
      : Stmt(NodeType::BreakStmt) { }
  } BreakStmt;

  Program* produceAST(char* code, size_t len);
private:
  Program program;
  Lexer* lexer;

  std::queue<Lexer::Token> tokens;
  size_t currentStmtIndex;

  BinaryExpr* binaryExprPool = new BinaryExpr[poolSize];
  Identifier* identifierPool = new Identifier[poolSize];
  NumericLiteral* numericLiteralPool = new NumericLiteral[poolSize];
  VarDeclaration* varDeclarationPool = new VarDeclaration[poolSize];
  AssignmentExpr* assignmentExprPool = new AssignmentExpr[poolSize];
  CallExpr* callExprPool = new CallExpr[poolSize];
  LogicalExpr* logicalExprPool = new LogicalExpr[poolSize];
  BlockStmt* blockStmtPool = new BlockStmt[poolSize];
  IfStmt* ifStmtPool = new IfStmt[poolSize];
  WhileStmt* whileStmtPool = new WhileStmt[poolSize];
  BreakStmt* breakStmtPool = new BreakStmt[poolSize];

  size_t binaryExprCount = 0;
  size_t identifierCount = 0;
  size_t numericLiteralCount = 0;
  size_t varDeclarationCount = 0;
  size_t assignmentExprCount = 0;
  size_t callExprCount = 0;
  size_t logicalExprCount = 0;
  size_t blockStmtCount = 0;
  size_t ifStmtCount = 0;
  size_t whileStmtCount = 0;
  size_t breakStmtCount = 0;

  BinaryExpr* newBinaryExpr();
  Identifier* newIdentifier();
  NumericLiteral* newNumericLiteral();
  VarDeclaration* newVarDeclaration();
  AssignmentExpr* newAssignmentExpr();
  CallExpr* newCallExpr();
  LogicalExpr* newLogicalExpr();
  BlockStmt* newBlockStmt();
  IfStmt* newIfStmt();
  WhileStmt* newWhileStmt();
  BreakStmt* newBreakStmt();

  void cleanup();

  Stmt* parseStmt();
  Expr* parseExpr();
  VarDeclaration* parseVarDeclaration();
  IfStmt* parseIfStmt();
  WhileStmt* parseWhileStmt();
  BreakStmt* parseBreakStmt();
  BlockStmt* parseBlockStmt();
  Expr* parseLogicalExpr();
  Expr* parseRelationalExpr();
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

  Lexer::Token at();
  Lexer::Token eat();
  Lexer::Token expect(Lexer::TokenType type, const char* expectedVal);
};