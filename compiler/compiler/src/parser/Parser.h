#pragma once

#include <map>
#include <memory>
#include <vector>

#include "lexer/Lexer.h"
#include "parser/AstNodes.h"
#include "ErrorHandler.h"

/**
 * @class Parser
 * 
 * A class responsible for parsing source code into an Abstract Syntax Tree (AST).
 * This class reads a tokenized input from the lexer and produces a hierarchical representation of the program's structure.
 */
class Parser {
public:
  /**
   * @brief Constructor for the Parser class.
   * @param lexer A pointer to the Lexer used for tokenizing the source code.
   */
  Parser() {
    this->lexer = new Lexer();
  }

  ~Parser() {
    delete lexer;
  }

  /**
   * @brief Produces the Abstract Syntax Tree (AST) for the provided source code.
   * @param code A pointer to the source code string.
   * @param len The length of the source code.
   * @return A pointer to the root AstNodes::Program node of the generated AST.
   */
  AstNodes::Program* produceAST(char* code, size_t len);

  /**
   * @brief Prints the AST in a human-readable format.
   * @param program A pointer to the root AstNodes::Program node of the AST.
   */
  void printAST(const AstNodes::Program* program);
private:
  AstNodes::Program program;       /**< The root program node of the AST */
  Lexer* lexer;                    /**< The lexer used for tokenizing the input */
  std::queue<Lexer::Token> tokens; /**< A queue of tokens to be processed */

  // Parsing functions for different types of statements and expressions
  std::unique_ptr<AstNodes::Stmt> parseStmt();
  std::unique_ptr<AstNodes::Expr> parseExpr();
  std::unique_ptr<AstNodes::VarDeclaration> parseVarDeclaration();
  std::unique_ptr<AstNodes::IfStmt> parseIfStmt();
  std::unique_ptr<AstNodes::WhileStmt> parseWhileStmt();
  std::unique_ptr<AstNodes::BreakStmt> parseBreakStmt();
  std::unique_ptr<AstNodes::BlockStmt> parseBlockStmt();
  std::unique_ptr<AstNodes::Expr> parseLogicalExpr();
  std::unique_ptr<AstNodes::Expr> parseRelationalExpr();
  std::unique_ptr<AstNodes::Expr> parseAssignmentExpr();
  std::unique_ptr<AstNodes::Expr> parseObjectExpr();
  std::unique_ptr<AstNodes::Expr> parseAdditiveExpr();
  std::unique_ptr<AstNodes::Expr> parseMultiplicativeExpr();
  std::unique_ptr<AstNodes::Expr> parseCallMemberExpr();
  std::unique_ptr<AstNodes::CallExpr> parseCallExpr(std::unique_ptr<AstNodes::Expr>&& caller);
  std::unique_ptr<AstNodes::Expr> parseMemberExpr();
  std::unique_ptr<AstNodes::Expr> parsePrimaryExpr();

  /**
   * @brief Checks if the end of the file (EOF) has been reached.
   * @return True if the EOF has been reached, otherwise false.
   */
  bool endOfFile();

  /**
   * @brief Pushes a statement to the program body.
   */
  void push(std::unique_ptr<AstNodes::Stmt>&& stmt);

  /**
   * @brief Synchronizes the parser by skipping tokens until a statement boundary is reached.
   */
  void synchronize();

  /**
   * @brief Looks up the `Token` at the front of the `tokens` queue.
   * @return The `Token` to be processed next.
   */
  Lexer::Token& at();

  /**
   * @brief Removes the `Token` at the front of the `tokens` queue.
   * @return The `Token` to be processed next.
   */
  Lexer::Token eat();

  /**
   * @brief Checks if the `Token` at the front of the `tokens` queue has the given `TokenType`.
   * @param type The expected `TokenType`.
   * @param errMsg The error message used if the given types do not equal.
   * @return The expected `Token`.
   * @throws ErrorHandler::restart with `expectedVal` and the actual value.
   */
  Lexer::Token expect(Lexer::TokenType type, const char* errMsg);

  // various toString functions
  void toStringNumericLiteral(const AstNodes::NumericLiteral* numLit);
  void toStringIdentifier(const AstNodes::Identifier* ident);
  void toStringStringLiteral(const AstNodes::StringLiteral* str);
  void toStringBinaryExpr(const AstNodes::BinaryExpr* binaryExpr);
  void toStringLogicalExpr(const AstNodes::LogicalExpr* logicalExpr);
  void toStringVarDecl(const AstNodes::VarDeclaration* varDecl);
  void toStringBlockStmt(const AstNodes::BlockStmt* blockStmt);
  void toStringIfStmt(const AstNodes::IfStmt* ifStmt);
  void toStringWhileStmt(const AstNodes::WhileStmt* whileStmt);
  void toStringBreakStmt(const AstNodes::BreakStmt* breakStmt);
  void toStringAssignmentExpr(const AstNodes::AssignmentExpr* assignmentExpr);
  void toStringObjectLiteral(const AstNodes::ObjectLiteral* objectLiteral);
  void toStringCallExpr(const AstNodes::CallExpr* callExpr);
  void toStringMemberExpr(const AstNodes::MemberExpr* memberExpr);
  void toString(const AstNodes::Stmt* stmt);
};