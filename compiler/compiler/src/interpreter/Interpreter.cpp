#include "Interpreter.h"

Values::RuntimeVal Interpreter::evaluate(Parser::Stmt* astNode, Environment* env) {
  ESP.wdtFeed();
  switch (astNode->kind) {
    case Parser::NodeType::NumericLiteral:
      {
        Values::NumberVal numVal(static_cast<Parser::NumericLiteral*>(astNode)->num);
        return numVal;
      }
    case Parser::NodeType::Identifier:
      return evalIdentifier(static_cast<Parser::Identifier*>(astNode), env);
    case Parser::NodeType::BinaryExpr:
      return evalBinaryExpr(static_cast<Parser::BinaryExpr* >(astNode), env);
    case Parser::NodeType::VarDeclaration:
      return evalVarDeclaration(static_cast<Parser::VarDeclaration* >(astNode), env);
    case Parser::NodeType::IfStmt:
      return evalIfStmt(static_cast<Parser::IfStmt* >(astNode), env);
    case Parser::NodeType::WhileStmt:
      return evalWhileStmt(static_cast<Parser::WhileStmt*>(astNode), env);
    case Parser::NodeType::BreakStmt:
      return evalBreakStmt(static_cast<Parser::BreakStmt*>(astNode), env);
    case Parser::NodeType::BlockStmt:
      return evalBlockStmt(static_cast<Parser::BlockStmt* >(astNode), env);
    case Parser::NodeType::LogicalExpr:
      return evalLogicalExpr(static_cast<Parser::LogicalExpr* >(astNode), env);
    case Parser::NodeType::AssignmentExpr:
      return evalAssignmentExpr(static_cast<Parser::AssignmentExpr*>(astNode), env);
    case Parser::NodeType::CallExpr:
      return evalCallExpr(static_cast<Parser::CallExpr*>(astNode), env);
    case Parser::NodeType::Program:
      return evalProgram(static_cast<Parser::Program*>(astNode), env);
    /*default:
      ErrorHandler::restart("AST node not set up for interpretation (should not happen)");
      break;*/
  }

  return Values::NullVal();
}

Values::RuntimeVal Interpreter::evalProgram(Parser::Program* program, Environment* env) {
  Values::RuntimeVal lastEvaluated = Values::NullVal();

  for (size_t i = 0; i < program->body.size(); i++) {
    lastEvaluated = evaluate(program->body[i], env);
    if (lastEvaluated.type == Values::ValueType::Break) {
      ErrorHandler::restart("A break statement may only be used within a loop");
    }
  }

  return lastEvaluated;
}

Values::RuntimeVal Interpreter::evalVarDeclaration(Parser::VarDeclaration* declaration, Environment* env) {
  Values::RuntimeVal val = declaration->value != nullptr ? evaluate(declaration->value, env) : Values::NullVal();
  return env->declareVar(declaration->ident, val, declaration->constant);
}

Values::RuntimeVal Interpreter::evalIfStmt(Parser::IfStmt* ifStmt, Environment* env) {
  Values::RuntimeVal result = evaluate(ifStmt->test, env);
  Serial.println("Evaluated test of if statement");
  if (result.type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Expected boolean value in if statement condition");
  } else {
    if (static_cast<Values::BooleanVal*>(&result)->value) {
      Serial.println("Evaluating consequent block");
      return evalBlockStmt(ifStmt->consequent, env);
    } else if (ifStmt->alternate != nullptr) {
      Serial.println("Evaluating alternate block");
      return evalBlockStmt(ifStmt->alternate, env);
    }
  }
  return Values::NullVal();
}

Values::RuntimeVal Interpreter::evalWhileStmt(Parser::WhileStmt* whileStmt, Environment* env) {
  Values::RuntimeVal testResult = evaluate(whileStmt->test, env);

  if (testResult.type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Expected boolean value in while statement condition");
  } else {
    Environment* childEnv = new Environment(env);
    Parser::BlockStmt* whileStmtBody = whileStmt->body;
    std::vector<Parser::Stmt*> blockStmtBody = whileStmtBody->body;
    Values::RuntimeVal result;

    while (true) {
      Values::RuntimeVal testResult = evaluate(whileStmt->test, env);
      if(!static_cast<Values::BooleanVal*>(&testResult)->value) {
        Serial.println("While test evaluated to false");  
        break;
      }

      for (size_t i = 0; i < blockStmtBody.size(); i++) {
        result = evaluate(blockStmtBody[i], childEnv);

        if (result.type == Values::ValueType::Break) {
          Serial.println("\"break\" found in \"while\" loop, jumping...");
          goto afterWhile;
        }
      }
    }
afterWhile:

    delete childEnv;
  }

  return Values::NullVal();
}

Values::BreakVal Interpreter::evalBreakStmt(Parser::BreakStmt* breakStmt, Environment* env) {
  return Values::BreakVal();
}

Values::RuntimeVal Interpreter::evalBlockStmt(Parser::BlockStmt* blockStmt, Environment* parent) {
  Environment* env = new Environment(parent);
  Values::RuntimeVal lastEvaluated;
  for (size_t i = 0; i < blockStmt->body.size(); i++) {
    lastEvaluated = evaluate(blockStmt->body[i], env);
    if (lastEvaluated.type == Values::ValueType::Break) break;
  }
  delete env;
  return lastEvaluated;
}

Values::BooleanVal Interpreter::evalLogicalExpr(Parser::LogicalExpr* logicalExpr, Environment* env) {
  Values::RuntimeVal left = evaluate(logicalExpr->left, env);
  Values::RuntimeVal right = evaluate(logicalExpr->right, env);

  Serial.println("Evaluated left and right part of logical expression");

  if (left.type != Values::ValueType::Boolean || right.type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Cannot use \"", logicalExpr->op, "\" on non-boolean values");
    return Values::BooleanVal(false);
  } else {
    Values::BooleanVal* leftBool = static_cast<Values::BooleanVal*>(&left);
    Values::BooleanVal* rightBool = static_cast<Values::BooleanVal*>(&right);

    Serial.println("Both boolean vals");

    Values::BooleanVal result = Values::BooleanVal();

    if (strcmp(logicalExpr->op, "and") == 0) {
      result.value = leftBool->value && rightBool->value;
    } else if (strcmp(logicalExpr->op, "or") == 0) {
      result.value = leftBool->value || rightBool->value;
    }

    Serial.print("Result: ");
    Serial.println(result.value ? "true" : "false");

    return result;
  }
}

Values::RuntimeVal Interpreter::evalBinaryExpr(Parser::BinaryExpr* binExp, Environment* env) {
  Values::RuntimeVal left = evaluate(binExp->left, env);
  Values::RuntimeVal right = evaluate(binExp->right, env);

  if (left.type == Values::ValueType::Number && right.type == Values::ValueType::Number) {
    Values::NumberVal* leftNum = static_cast<Values::NumberVal*>(&left);
    Values::NumberVal* rightNum = static_cast<Values::NumberVal*>(&right);

    return evalNumericBinaryExpr(leftNum, rightNum, binExp->op, env);
  } else if (left.type == Values::ValueType::Boolean && right.type == Values::ValueType::Boolean) {
    Values::BooleanVal* leftBool = static_cast<Values::BooleanVal*>(&left);
    Values::BooleanVal* rightBool = static_cast<Values::BooleanVal*>(&right);

    return evalBooleanBinaryExpr(leftBool, rightBool, binExp->op, env);
  } else {
    ErrorHandler::restart("Value types not compatible");
  }

  return Values::NullVal();
}

Values::RuntimeVal Interpreter::evalIdentifier(Parser::Identifier* ident, Environment* env) {
  Values::RuntimeVal val = env->lookupVar(ident->symbol);
  return val;
}

Values::RuntimeVal Interpreter::evalNumericBinaryExpr(Values::NumberVal* left, Values::NumberVal* right, char* op, Environment* env) {
  Values::NumberVal numberVal = Values::NumberVal();

  switch (op[0]) {
    case '+':
      numberVal.value = left->value + right->value;
      break;
    case '-':
      numberVal.value = left->value - right->value;
      break;
    case '*':
      numberVal.value = left->value * right->value;
      break;
    case '/':
      if (right->value == 0) {
        ErrorHandler::restart("Attempted to divide by 0");
      } else {
        numberVal.value = left->value / right->value;
      }
      break;
    case '%':
      numberVal.value = (float)((int)left->value % (int)right->value);
      break;
    default:
      {
        Values::BooleanVal boolVal = Values::BooleanVal();
        if (strcmp(op, "<") == 0) {
          boolVal.value = left->value < right->value;
        } else if (strcmp(op, "<=") == 0) {
          boolVal.value = left->value <= right->value;
        } else if (strcmp(op, ">") == 0) {
          boolVal.value = left->value > right->value;
        } else if (strcmp(op, ">=") == 0) {
          boolVal.value = left->value >= right->value;
        } else if (strcmp(op, "==") == 0) {
          boolVal.value = left->value == right->value;
        } else if (strcmp(op, "!=") == 0) {
          boolVal.value = left->value != right->value;
        } else {
          ErrorHandler::restart("Unknown operator \"", op, "\" encountered while interpreting");
        }
        return boolVal;
      }
      break;
  }

  return numberVal;
}

Values::BooleanVal Interpreter::evalBooleanBinaryExpr(Values::BooleanVal* left, Values::BooleanVal* right, char* op, Environment* env) {
  Values::BooleanVal boolVal = Values::BooleanVal();

  if (strcmp(op, "==") == 0) {
    boolVal.value = left->value == right->value;
  } else if (strcmp(op, "!=") == 0) {
    boolVal.value = left->value != right->value;
  } else {
    ErrorHandler::restart("Cannot compare two Booleans with \"", op, "\"");
  }

  return boolVal;
}

Values::RuntimeVal Interpreter::evalAssignmentExpr(Parser::AssignmentExpr* node, Environment* env) {
  if (node->assignee->kind != Parser::NodeType::Identifier) {
    ErrorHandler::restart("Expected identifier on left side of assignment expression");
  }

  const char* varname = static_cast<Parser::Identifier*>(node->assignee)->symbol;

  return env->assignVar(varname, evaluate(node->value, env));
}

Values::RuntimeVal Interpreter::evalCallExpr(Parser::CallExpr* expr, Environment* env) {
  Serial.println("evalCallExpr");
  std::vector<Values::RuntimeVal> args;
  if (!expr->args.empty()) {
    args.reserve(expr->args.size());

    for (size_t i = 0; i < args.size(); i++) {
      Serial.println("Found function argument");
      args.push_back(evaluate(expr->args[i], env));
    }
  }

  Values::RuntimeVal fn = evaluate(expr->caller, env);

  Serial.println("Evaluated callExpr");

  if (fn.type != Values::ValueType::NativeFn) {
    ErrorHandler::restart("Cannot call value that is not a function");
  }

  Values::NativeFnVal* function = static_cast<Values::NativeFnVal*>(&fn);
  
  Serial.print("Found function ");
  Serial.println(static_cast<Parser::Identifier*>(expr->caller)->symbol);

  Values::RuntimeVal result = function->call(args, env);
  return result;
}
