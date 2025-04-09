#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Lexer.h"
#include "AstNodes.h"
#include "ErrorHandler.h"

/**
 * @class Parser
 * 
 * A class responsible for parsing source code into an Abstract Syntax Tree (AST).
 * This class reads a tokenized input from the lexer and produces a hierarchical representation of the program's structure.
 */
class Parser {
public:
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

  /**
   * @brief Parses a statement from the token stream.
   * @return A unique pointer to the parsed statement.
   */
  std::unique_ptr<AstNodes::Stmt> parseStmt();

  /**
   * @brief Parses an expression from the token stream.
   * @return A unique pointer to the parsed expression.
   */
  std::unique_ptr<AstNodes::Expr> parseExpr();

  /**
   * @brief Parses a variable declaration statement.
   * @return A unique pointer to the parsed variable declaration.
   */
  std::unique_ptr<AstNodes::VarDeclaration> parseVarDeclaration();

  /**
   * @brief Parses an if statement.
   * @return A unique pointer to the parsed if statement.
   */
  std::unique_ptr<AstNodes::IfStmt> parseIfStmt();

  /**
   * @brief Parses a while statement.
   * @return A unique pointer to the parsed while statement.
   */
  std::unique_ptr<AstNodes::WhileStmt> parseWhileStmt();

  /**
   * @brief Parses a break statement.
   * @return A unique pointer to the parsed break statement.
   */
  std::unique_ptr<AstNodes::BreakStmt> parseBreakStmt();

  /**
   * @brief Parses a block statement.
   * @return A unique pointer to the parsed block statement.
   */
  std::unique_ptr<AstNodes::BlockStmt> parseBlockStmt();

  /**
   * @brief Parses a logical expression.
   * Examples include 'a and b' or 'x or y'.
   * @return A unique pointer to the parsed logical expression.
   */
  std::unique_ptr<AstNodes::Expr> parseLogicalExpr();

  /**
   * @brief Parses a relational expression.
   * Examples of relational expressions include '<', '<=', '>', '>=', '==', and '!='.
   * @return A unique pointer to the parsed relational expression.
   */
  std::unique_ptr<AstNodes::Expr> parseRelationalExpr();

  /**
   * @brief Parses an assignment expression.
   * @return A unique pointer to the parsed assignment expression.
   */
  std::unique_ptr<AstNodes::Expr> parseAssignmentExpr();

  /**
   * @brief Parses an object expression.
   * @return A unique pointer to the parsed object expression.
   */
  std::unique_ptr<AstNodes::Expr> parseObjectExpr();

  /**
   * @brief Parses an array expression.
   * @return A unique pointer to the parsed array expression.
   */
  std::unique_ptr<AstNodes::Expr> parseArrayExpr();

  /**
   * @brief Parses an additive expression (e.g., addition and subtraction).
   * 
   * Examples include 'a + b' or 'x - y'.
   * 
   * @return A unique pointer to the parsed additive expression.
   */
  std::unique_ptr<AstNodes::Expr> parseAdditiveExpr();

  /**
   * @brief Parses a multiplicative expression (e.g., multiplication, division, and modulo).
   * 
   * Examples include 'a * b', 'x / y', or 'z % 2'.
   * 
   * @return A unique pointer to the parsed multiplicative expression.
   */
  std::unique_ptr<AstNodes::Expr> parseMultiplicativeExpr();

  /**
   * @brief Parses a call or member expression.
   * @return A unique pointer to the parsed call or member expression.
   */
  std::unique_ptr<AstNodes::Expr> parseCallMemberExpr();

  /**
   * @brief Parses a function call expression.
   * @param caller A unique pointer to the caller expression.
   * @return A unique pointer to the parsed call expression.
   */
  std::unique_ptr<AstNodes::CallExpr> parseCallExpr(std::unique_ptr<AstNodes::Expr>&& caller);

  /**
   * @brief Parses a member expression.
   * 
   * Examples include 'object.property' or 'array[index]'.
   * 
   * @return A unique pointer to the parsed member expression.
   */
  std::unique_ptr<AstNodes::Expr> parseMemberExpr();

  /**
   * @brief Parses a primary expression (e.g., literals, identifiers, or grouped expressions).
   * 
   * Examples include '42', '"hello"', or '(a + b)'.
   * 
   * @return A unique pointer to the parsed primary expression.
   */
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
  void toStringArrayLiteral(const AstNodes::ArrayLiteral* arrayLiteral);
  void toStringCallExpr(const AstNodes::CallExpr* callExpr);
  void toStringMemberExpr(const AstNodes::MemberExpr* memberExpr);
  void toStringNodeType(const AstNodes::NodeType type);
  void toString(const AstNodes::Stmt* stmt);
};