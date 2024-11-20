#include "Interpreter.h"

Values::RuntimeVal* Interpreter::evaluate(Parser::Stmt* astNode, Environment* env) {
  switch (astNode->kind) {
    case Parser::NodeType::NumericLiteral:
      {
        Serial.println("NumLit");
        Values::NumberVal* numVal = env->values.newNumberVal();
        Parser::NumericLiteral* numericLiteral = static_cast<Parser::NumericLiteral*>(astNode);

        numVal->value = numericLiteral->num;

        return numVal;
      }
    case Parser::NodeType::Identifier:
      Serial.println("Ident");
      return evalIdentifier(static_cast<Parser::Identifier*>(astNode), env);
    case Parser::NodeType::BinaryExpr:
      Serial.println("BinExp");
      return evalBinaryExpr(static_cast<Parser::BinaryExpr* >(astNode), env);
    case Parser::NodeType::VarDeclaration:
      Serial.println("VarDecl");
      return evalVarDeclaration(static_cast<Parser::VarDeclaration* >(astNode), env);
    case Parser::NodeType::Program:
      Serial.println("Program");
      return evalProgram(static_cast<Parser::Program*>(astNode), env);
    default:
      GlobalFunctions::restart("AST node not set up for interpretation (should not happen)");
      return env->values.newNullVal();
  }
}

Values::RuntimeVal* Interpreter::evalProgram(Parser::Program* program, Environment* env) {
  Values::RuntimeVal* lastEvaluated = env->values.newNullVal();

  for (size_t i = 0; i < maxProgramStatements && program->body[i] != nullptr; i++) {
    lastEvaluated = evaluate(program->body[i], env);
  }

  return lastEvaluated;
}

Values::RuntimeVal* Interpreter::evalVarDeclaration(Parser::VarDeclaration* declaration, Environment* env) {
  Values::RuntimeVal* val = declaration->value != nullptr ? evaluate(declaration->value, env) : env->values.newNullVal();
  return env->declareVar(declaration->ident, val);
}

Values::RuntimeVal* Interpreter::evalBinaryExpr(Parser::BinaryExpr* binExp, Environment* env) {
  Values::RuntimeVal* left = evaluate(binExp->left, env);
  Values::RuntimeVal* right = evaluate(binExp->right, env);

  if (left->type == Values::ValueType::Number && right->type == Values::ValueType::Number) {
    Values::NumberVal* leftNum = static_cast<Values::NumberVal*>(left);
    Values::NumberVal* rightNum = static_cast<Values::NumberVal*>(right);

    return evalNumericBinaryExpr(leftNum, rightNum, binExp->op, env);
  }

  return env->values.newNullVal();
}

Values::RuntimeVal* Interpreter::evalIdentifier(Parser::Identifier* ident, Environment* env) {
  Values::RuntimeVal* val = env->lookupVar(ident->symbol);
  return val;
}

Values::NumberVal* Interpreter::evalNumericBinaryExpr(Values::NumberVal* left, Values::NumberVal* right, char op, Environment* env) {
  Values::NumberVal* numberVal = env->values.newNumberVal();

  switch (op) {
    case '+':
      numberVal->value = left->value + right->value;
      break;
    case '-':
      numberVal->value = left->value - right->value;
      break;
    case '*':
      numberVal->value = left->value * right->value;
      break;
    case '/':
      if(right->value == 0) {
        GlobalFunctions::restart("Attempted to divide by 0");
      } else {
        numberVal->value = left->value / right->value;
      }
      break;
    case '%':
      numberVal->value = (float)((int)left->value % (int)right->value);
      break;
    default:
      {
        char c[] = { op, '\0' };
        GlobalFunctions::restart("Unknown binary operator \"", c, "\" encountered while interpreting");
      }
      break;
  }

  return numberVal;
}