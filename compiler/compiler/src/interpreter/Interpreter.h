#pragma once

#include "GlobalFunctions.h"
#include "parser/Parser.h"
#include "Values.h"
#include "Environment.h"

class Interpreter {
public:
  Values::RuntimeVal* evaluate(Parser::Stmt* astNode, Environment* env);
private:
  Values::RuntimeVal* evalProgram(Parser::Program* program, Environment* env);
  Values::RuntimeVal* evalBinaryExpr(Parser::BinaryExpr* binExp, Environment* env);
  Values::RuntimeVal* evalVarDeclaration(Parser::VarDeclaration* declaration, Environment* env);
  void evalIfStmt(Parser::IfStmt* ifStmt, Environment* env);
  void evalBlockStmt(Parser::BlockStmt* blockStmt, Environment* parent);
  Values::BooleanVal* evalLogicalExpr(Parser::LogicalExpr* logicalExpr, Environment* env);
  Values::RuntimeVal* evalIdentifier(Parser::Identifier* ident, Environment* env);
  Values::RuntimeVal* evalNumericBinaryExpr(Values::NumberVal* left, Values::NumberVal* right, char* op, Environment* env);
  Values::BooleanVal* evalBooleanBinaryExpr(Values::BooleanVal* left, Values::BooleanVal* right, char* op, Environment* env);
  Values::RuntimeVal* evalAssignmentExpr(Parser::AssignmentExpr* node, Environment* env);
  Values::RuntimeVal* evalCallExpr(Parser::CallExpr* expr, Environment* env);
};