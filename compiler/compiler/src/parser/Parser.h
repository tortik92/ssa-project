#pragma once

#include "lexer/Lexer.h"
#include "GlobalFunctions.h"

class Parser {
public:
  Parser() {
    tokensPtr = nullptr; // gets assigned in produceAST()
    currentStmtIndex = 0;
  }

  enum class NodeType {
    Undefined,
    Program,
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
        for(size_t i = 0; i < maxProgramStatements; i++) {
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

  typedef struct BinaryExpr : Expr {
    Expr* left;
    Expr* right;
    char op[2];

    BinaryExpr()
      : Expr(NodeType::BinaryExpr) {}
  } BinaryExpr;

  typedef struct Identifier : Expr {
    char symbol[tokLen];

    Identifier()
      : Expr(NodeType::Identifier) {}
  } Identifier;

  typedef struct NumericLiteral : Expr {
    float num;

    NumericLiteral()
      : Expr(NodeType::NumericLiteral) {}
  } NumericLiteral;

  Program* produceAST(char* code, size_t len);
private:
  Program program;
  Lexer lexer;

  Lexer::Token* tokensPtr;
  size_t currentStmtIndex;

  BinaryExpr binaryExprPool[poolSize];
  Identifier identifierPool[poolSize];
  NumericLiteral numericLiteralPool[poolSize]; 
  size_t binaryExprCount = 0;
  size_t identifierCount = 0;
  size_t numericLiteralCount = 0;

  void clearPools();
  BinaryExpr* newBinaryExpr();
  Identifier* newIdentifier();
  NumericLiteral* newNumericLiteral();

  /**
   * orders of expression:
   * 
   * 1. additive
   * 2. multiplicative
   * 3. primary
   */
  Stmt* parseStmt();
  Expr* parseExpr();
  Expr* parseAdditiveExpr();
  Expr* parseMultiplicativeExpr();
  Expr* parsePrimaryExpr();

  bool endOfFile();
  void push(Stmt* stmt);

  Lexer::Token* at();
  Lexer::Token* eat();
  Lexer::Token* expect(Lexer::TokenType type, const char* err);
};