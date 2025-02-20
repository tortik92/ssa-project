#pragma once

#include "ErrorHandler.h"
#include "parser/Parser.h"
#include "Values.h"
#include "Environment.h"

/**
 * @class Interpreter
 * @brief The Interpreter class is responsible for evaluating AST (abstract syntax tree) nodes
 * 
 * These nodes are generated by the parser and executing the respective statements and expressions 
 * in the context of a given environment.
 */
class Interpreter {
public:
  /**
   * @brief Evaluates an AST node and returns the resulting value in the given environment.
   * 
   * @param astNode The AST node to be evaluated.
   * @param env The environment in which the AST node is evaluated.
   * @return The runtime value resulting from the evaluation of the AST node.
   */
  Values::RuntimeVal evaluate(Parser::Stmt* astNode, Environment* env);

private:
  /**
   * @brief Evaluates a program (a series of statements) in the given environment.
   * 
   * @param program The program to be evaluated.
   * @param env The environment in which the program is evaluated.
   * @return The last evaluated value from the program body.
   */
  Values::RuntimeVal evalProgram(Parser::Program* program, Environment* env);

  /**
   * @brief Evaluates a binary expression in the given environment.
   * 
   * @param binExp The binary expression to be evaluated.
   * @param env The environment in which the expression is evaluated.
   * @return The resulting value from evaluating the binary expression.
   */
  Values::RuntimeVal evalBinaryExpr(Parser::BinaryExpr* binExp, Environment* env);

  /**
   * @brief Evaluates a variable declaration in the given environment.
   * 
   * @param declaration The variable declaration to be evaluated.
   * @param env The environment in which the declaration is evaluated.
   * @return The resulting value from declaring the variable.
   */
  Values::RuntimeVal evalVarDeclaration(Parser::VarDeclaration* declaration, Environment* env);

  /**
   * @brief Evaluates an if statement in the given environment.
   * 
   * @param ifStmt The if statement to be evaluated.
   * @param env The environment in which the if statement is evaluated.
   * @return The result of evaluating the consequent or alternate block.
   */
  Values::RuntimeVal evalIfStmt(Parser::IfStmt* ifStmt, Environment* env);

  /**
   * @brief Evaluates a while statement in the given environment.
   * 
   * @param whileStmt The while statement to be evaluated.
   * @param env The environment in which the while statement is evaluated.
   * @return The result of evaluating the while loop.
   */
  Values::RuntimeVal evalWhileStmt(Parser::WhileStmt* whileStmt, Environment* env);

  /**
   * @brief Evaluates a break statement in the given environment.
   * 
   * @param breakStmt The break statement to be evaluated.
   * @param env The environment in which the break statement is evaluated.
   * @return A break value indicating the break action.
   */
  Values::BreakVal evalBreakStmt(Parser::BreakStmt* breakStmt, Environment* env);

  /**
   * @brief Evaluates a block of statements in the given environment.
   * 
   * @param blockStmt The block statement to be evaluated.
   * @param parent The environment of the parent block.
   * @return The result of the last evaluated statement in the block.
   */
  Values::RuntimeVal evalBlockStmt(Parser::BlockStmt* blockStmt, Environment* parent);

  /**
   * @brief Evaluates a logical expression in the given environment.
   * 
   * @param logicalExpr The logical expression to be evaluated.
   * @param env The environment in which the logical expression is evaluated.
   * @return The resulting boolean value from evaluating the logical expression.
   */
  Values::BooleanVal evalLogicalExpr(Parser::LogicalExpr* logicalExpr, Environment* env);

  /**
   * @brief Evaluates an identifier (variable) in the given environment.
   * 
   * @param ident The identifier to be evaluated.
   * @param env The environment in which the identifier is evaluated.
   * @return The value associated with the identifier in the environment.
   */
  Values::RuntimeVal evalIdentifier(Parser::Identifier* ident, Environment* env);

  /**
   * @brief Evaluates a numeric binary expression (e.g., addition, subtraction) in the given environment.
   * 
   * @param left The left operand of the binary expression.
   * @param right The right operand of the binary expression.
   * @param op The operator of the binary expression.
   * @param env The environment in which the expression is evaluated.
   * @return The resulting value from evaluating the binary expression.
   */
  Values::RuntimeVal evalNumericBinaryExpr(Values::NumberVal* left, Values::NumberVal* right, char* op, Environment* env);

  /**
   * @brief Evaluates a boolean binary expression (e.g., equality, inequality) in the given environment.
   * 
   * @param left The left operand of the binary expression.
   * @param right The right operand of the binary expression.
   * @param op The operator of the binary expression.
   * @param env The environment in which the expression is evaluated.
   * @return The resulting boolean value from evaluating the binary expression.
   */
  Values::BooleanVal evalBooleanBinaryExpr(Values::BooleanVal* left, Values::BooleanVal* right, char* op, Environment* env);

  /**
   * @brief Evaluates a binary expression of two strings (e.g., equality, inequality) in the given environment.
   * 
   * @param left The left operand of the binary expression.
   * @param right The right operand of the binary expression.
   * @param op The operator of the binary expression.
   * @param env The environment in which the expression is evaluated.
   * @return The resulting boolean value from evaluating the binary expression.
   */
  Values::BooleanVal evalStringBinaryExpr(Values::StringVal* left, Values::StringVal* right, char* op, Environment* env);

  /**
   * @brief Evaluates an assignment expression in the given environment.
   * 
   * @param node The assignment expression to be evaluated.
   * @param env The environment in which the assignment expression is evaluated.
   * @return The result of the assignment expression evaluation.
   */
  Values::RuntimeVal evalAssignmentExpr(Parser::AssignmentExpr* node, Environment* env);

  /**
   * @brief Evaluates an object expression in the given environment.
   * 
   * @param obj The object expression to be evaluated.
   * @param env The environment in which the assignment expression is evaluated.
   * @return The result of the object expression evaluation.
   */
  Values::ObjectVal evalObjectExpr(Parser::ObjectLiteral* obj, Environment* env);

  /**
   * @brief Evaluates a function call expression in the given environment.
   * 
   * @param expr The call expression to be evaluated.
   * @param env The environment in which the call expression is evaluated.
   * @return The result of evaluating the function call.
   */
  Values::RuntimeVal evalCallExpr(Parser::CallExpr* expr, Environment* env);

  /**
   * @brief Evaluates a member expression in the given environment.
   * 
   * @param expr The member expression to be evaluated.
   * @param env The environment in which the member expression is evaluated.
   * @return The result of evaluating the member expression.
   */
  Values::RuntimeVal evalMemberExpr(Parser::MemberExpr* member, Environment* env);
};