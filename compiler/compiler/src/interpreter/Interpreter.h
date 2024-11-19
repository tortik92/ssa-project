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
  Values::RuntimeVal* evalIdentifier(Parser::Identifier* ident, Environment* env);
  Values::NumberVal* evalNumericBinaryExpr(Values::NumberVal* left, Values::NumberVal* right, char op, Environment* env);
};