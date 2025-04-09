#include "Interpreter.h"

std::unique_ptr<Values::RuntimeVal> Interpreter::evaluate(const AstNodes::Stmt* astNode, Environment* env) {
  yield();
  switch (astNode->kind) {
    case AstNodes::NodeType::NumericLiteral:
      return std::make_unique<Values::NumberVal>(static_cast<const AstNodes::NumericLiteral*>(astNode)->num);
    case AstNodes::NodeType::StringLiteral:
      return std::make_unique<Values::StringVal>(static_cast<const AstNodes::StringLiteral*>(astNode)->value);
    case AstNodes::NodeType::Identifier:
      return evalIdentifier(static_cast<const AstNodes::Identifier*>(astNode), env);
    case AstNodes::NodeType::BinaryExpr:
      return evalBinaryExpr(static_cast<const AstNodes::BinaryExpr* >(astNode), env);
    case AstNodes::NodeType::VarDeclaration:
      return evalVarDeclaration(static_cast<const AstNodes::VarDeclaration* >(astNode), env);
    case AstNodes::NodeType::IfStmt:
      return evalIfStmt(static_cast<const AstNodes::IfStmt* >(astNode), env);
    case AstNodes::NodeType::WhileStmt:
      return evalWhileStmt(static_cast<const AstNodes::WhileStmt*>(astNode), env);
    case AstNodes::NodeType::BreakStmt:
      return evalBreakStmt(static_cast<const AstNodes::BreakStmt*>(astNode), env);
    case AstNodes::NodeType::BlockStmt:
      return evalBlockStmt(static_cast<const AstNodes::BlockStmt* >(astNode), env);
    case AstNodes::NodeType::LogicalExpr:
      return evalLogicalExpr(static_cast<const AstNodes::LogicalExpr* >(astNode), env);
    case AstNodes::NodeType::AssignmentExpr:
      return evalAssignmentExpr(static_cast<const AstNodes::AssignmentExpr*>(astNode), env);
    case AstNodes::NodeType::ObjectLiteral:
      return evalObjectExpr(static_cast<const AstNodes::ObjectLiteral*>(astNode), env);
    case AstNodes::NodeType::ArrayLiteral:
      return evalArrayExpr(static_cast<const AstNodes::ArrayLiteral*>(astNode), env);
    case AstNodes::NodeType::CallExpr:
      return evalCallExpr(static_cast<const AstNodes::CallExpr*>(astNode), env);
    case AstNodes::NodeType::MemberExpr:
      return evalMemberExpr(static_cast<const AstNodes::MemberExpr*>(astNode), env);
    case AstNodes::NodeType::Program:
      return evalProgram(static_cast<const AstNodes::Program*>(astNode), env);
  }

  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalProgram(const AstNodes::Program* program, Environment* env) {
  std::unique_ptr<Values::RuntimeVal> lastEvaluated = std::make_unique<Values::NullVal>();

  for (size_t i = 0; i < program->body.size(); i++) {
    lastEvaluated = evaluate(program->body[i].get(), env);
    if (lastEvaluated->type == Values::ValueType::Break) {
      ErrorHandler::restart("A break statement may only be used within a loop");
    }
  }

  return lastEvaluated;
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalVarDeclaration(const AstNodes::VarDeclaration* declaration, Environment* env) {
  Serial.println("evalVarDeclaration");
  std::unique_ptr<Values::RuntimeVal> val = declaration->value ? evaluate(declaration->value.get(), env) : std::make_unique<Values::NullVal>();
  return env->declareVar(declaration->ident, std::move(val), declaration->constant);
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalIfStmt(const AstNodes::IfStmt* ifStmt, Environment* env) {
  std::unique_ptr<Values::RuntimeVal> result = evaluate(ifStmt->test.get(), env);
  Serial.println("Evaluated test of if statement");
  if (result->type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Expected boolean value in if statement condition");
  } else {
    if (static_cast<Values::BooleanVal*>(result.get())->value) {
      Serial.println("Evaluating consequent block");
      return evalBlockStmt(ifStmt->consequent.get(), env);
    } else if (ifStmt->alternate != nullptr) {
      Serial.println("Evaluating alternate block");
      return evalBlockStmt(ifStmt->alternate.get(), env);
    }
  }
  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalWhileStmt(const AstNodes::WhileStmt* whileStmt, Environment* env) {
  std::unique_ptr<Values::RuntimeVal> testResult = evaluate(whileStmt->test.get(), env);

  if (testResult->type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Expected boolean value in while statement condition");
  } else {
    
    AstNodes::BlockStmt* whileStmtBody = whileStmt->body.get();
    std::vector<std::unique_ptr<AstNodes::Stmt>>& blockStmtBody = whileStmtBody->body;
    std::unique_ptr<Values::RuntimeVal> result;

    while (true) {
      
      std::unique_ptr<Values::RuntimeVal> testResult = evaluate(whileStmt->test.get(), env);
      if (!static_cast<Values::BooleanVal*>(testResult.get())->value) {
        Serial.println("While test evaluated to false");
        break;
      }

      Environment* childEnv = new Environment(env);
      for (size_t i = 0; i < blockStmtBody.size(); i++) {
        result = evaluate(blockStmtBody[i].get(), childEnv);

        if (result->type == Values::ValueType::Break) {
          Serial.println("\"break\" found in \"while\" loop, jumping...");
          delete childEnv;
          return std::make_unique<Values::NullVal>();
        }
      }
      delete childEnv;
    }
  }

  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::BreakVal> Interpreter::evalBreakStmt(const AstNodes::BreakStmt* breakStmt, Environment* env) {
  Serial.println("evalBreakStmt");
  return std::make_unique<Values::BreakVal>();
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalBlockStmt(const AstNodes::BlockStmt* blockStmt, Environment* parent) {
  Serial.println("evalBlockStmt");
  Environment* env = new Environment(parent);
  std::unique_ptr<Values::RuntimeVal> lastEvaluated;
  for (size_t i = 0; i < blockStmt->body.size(); i++) {
    lastEvaluated = evaluate(blockStmt->body[i].get(), env);
    if (lastEvaluated->type == Values::ValueType::Break) break;
  }
  delete env;
  return lastEvaluated;
}

std::unique_ptr<Values::BooleanVal> Interpreter::evalLogicalExpr(const AstNodes::LogicalExpr* logicalExpr, Environment* env) {
  Serial.println("evalLogicalExpr");
  std::unique_ptr<Values::RuntimeVal> left = evaluate(logicalExpr->left.get(), env);
  std::unique_ptr<Values::RuntimeVal> right = evaluate(logicalExpr->right.get(), env);

  Serial.println("Evaluated left and right part of logical expression");

  if (left->type != Values::ValueType::Boolean || right->type != Values::ValueType::Boolean) {
    ErrorHandler::restart("Cannot use \"", logicalExpr->op, "\" on non-boolean values");
    return std::make_unique<Values::BooleanVal>(false);
  } else {
    const Values::BooleanVal* leftBool = static_cast<const Values::BooleanVal*>(left.get());
    const Values::BooleanVal* rightBool = static_cast<const Values::BooleanVal*>(right.get());

    Serial.println("Both boolean vals");

    std::unique_ptr<Values::BooleanVal> result = std::make_unique<Values::BooleanVal>();

    if (strcmp(logicalExpr->op, "and") == 0) {
      result->value = leftBool->value && rightBool->value;
    } else if (strcmp(logicalExpr->op, "or") == 0) {
      result->value = leftBool->value || rightBool->value;
    }

    Serial.print("Result: ");
    Serial.println(result->value ? "true" : "false");

    return result;
  }
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalBinaryExpr(const AstNodes::BinaryExpr* binExp, Environment* env) {
  Serial.println("evalBinaryExpr");
  std::unique_ptr<Values::RuntimeVal> left = evaluate(binExp->left.get(), env);
  std::unique_ptr<Values::RuntimeVal> right = evaluate(binExp->right.get(), env);

  if (left->type == Values::ValueType::Number && right->type == Values::ValueType::Number) {
    const Values::NumberVal* leftNum = static_cast<const Values::NumberVal*>(left.get());
    const Values::NumberVal* rightNum = static_cast<const Values::NumberVal*>(right.get());

    return evalNumericBinaryExpr(leftNum, rightNum, binExp->op, env);
  } else if (left->type == Values::ValueType::Boolean && right->type == Values::ValueType::Boolean) {
    const Values::BooleanVal* leftBool = static_cast<const Values::BooleanVal*>(left.get());
    const Values::BooleanVal* rightBool = static_cast<const Values::BooleanVal*>(right.get());

    return evalBooleanBinaryExpr(leftBool, rightBool, binExp->op, env);
  } else if (left->type == Values::ValueType::String && right->type == Values::ValueType::String) {
    const Values::StringVal* leftString = static_cast<const Values::StringVal*>(left.get());
    const Values::StringVal* rightString = static_cast<const Values::StringVal*>(right.get());

    return evalStringBinaryExpr(leftString, rightString, binExp->op, env);
  } else {
    ErrorHandler::noComparisonPossible(Values::getString(left->type).c_str(), Values::getString(right->type).c_str());
  }

  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalIdentifier(const AstNodes::Identifier* ident, Environment* env) {
  Serial.println("evalIdentifier");
  std::unique_ptr<Values::RuntimeVal> val = env->lookupVar(ident->symbol);
  return val;
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalNumericBinaryExpr(const Values::NumberVal* left, const Values::NumberVal* right, char* op, Environment* env) {
  Serial.println("evalNumericBinaryExpr");
  std::unique_ptr<Values::NumberVal> numberVal = std::make_unique<Values::NumberVal>();

  Serial.print("Left value: ");
  Serial.println(left->value);

  Serial.print("Right value: ");
  Serial.println(right->value);

  switch (op[0]) {
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
      if (right->value == 0) {
        ErrorHandler::restart("Attempted to divide by 0");
      } else {
        numberVal->value = left->value / right->value;
      }
      break;
    case '%':
      numberVal->value = (float)((int)left->value % (int)right->value);
      break;
    default:
      {
        std::unique_ptr<Values::BooleanVal> boolVal = std::make_unique<Values::BooleanVal>();
        if (strcmp(op, "<") == 0) {
          boolVal->value = left->value < right->value;
        } else if (strcmp(op, "<=") == 0) {
          boolVal->value = left->value <= right->value;
        } else if (strcmp(op, ">") == 0) {
          boolVal->value = left->value > right->value;
        } else if (strcmp(op, ">=") == 0) {
          boolVal->value = left->value >= right->value;
        } else if (strcmp(op, "==") == 0) {
          boolVal->value = left->value == right->value;
        } else if (strcmp(op, "!=") == 0) {
          boolVal->value = left->value != right->value;
        } else {
          ErrorHandler::restart("Unknown operator \"", op, "\" encountered while interpreting");
        }
        return boolVal;
      }
      break;
  }

  return numberVal;
}

std::unique_ptr<Values::BooleanVal> Interpreter::evalBooleanBinaryExpr(const Values::BooleanVal* left, const Values::BooleanVal* right, char* op, Environment* env) {
  Serial.println("evalBooleanBinaryExpr");
  std::unique_ptr<Values::BooleanVal> boolVal = std::make_unique<Values::BooleanVal>();

  if (strcmp(op, "==") == 0) {
    boolVal->value = left->value == right->value;
  } else if (strcmp(op, "!=") == 0) {
    boolVal->value = left->value != right->value;
  } else {
    ErrorHandler::restart("Cannot compare two Booleans with \"", op, "\"");
  }

  return boolVal;
}

std::unique_ptr<Values::BooleanVal> Interpreter::evalStringBinaryExpr(const Values::StringVal* left, const Values::StringVal* right, char* op, Environment* env) {
  Serial.println("evalStringBinaryExpr");
  std::unique_ptr<Values::BooleanVal> boolVal = std::make_unique<Values::BooleanVal>();

  if (strcmp(op, "==") == 0) {
    boolVal->value = strcmp(left->str, right->str) == 0;
  } else if (strcmp(op, "!=") == 0) {
    boolVal->value = strcmp(left->str, right->str) != 0;
  } else {
    ErrorHandler::restart("Cannot compare two Strings with \"", op, "\"");
  }

  return boolVal;
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalAssignmentExpr(const AstNodes::AssignmentExpr* assignmentExpr, Environment* env) {
  Serial.println("evalAssignmentExpr");
  switch (assignmentExpr->assignee->kind) {
    {
      case AstNodes::NodeType::Identifier:
        {
          const char* varname = static_cast<const AstNodes::Identifier*>(assignmentExpr->assignee.get())->symbol;
          return env->assignVar(varname, evaluate(assignmentExpr->value.get(), env));
        }
      case AstNodes::NodeType::MemberExpr:
        {
          const AstNodes::MemberExpr* member = static_cast<const AstNodes::MemberExpr*>(assignmentExpr->assignee.get());
          const char* varname = static_cast<const AstNodes::Identifier*>(member->object.get())->symbol;
          std::unique_ptr<Values::RuntimeVal> memberVal = evaluate(member->object.get(), env);
          std::unique_ptr<Values::RuntimeVal> propertyVal = evaluate(member->property.get(), env);

          switch (memberVal->type) {
            case Values::ValueType::ArrayVal:
              {
                Values::ArrayVal* array = static_cast<Values::ArrayVal*>(memberVal.get());
                if (!member->computed) {
                  ErrorHandler::restart("Cannot perform member access with '.' on array value");
                }

                if (propertyVal->type != Values::ValueType::Number) {
                  ErrorHandler::restart("Computed property must evaluate to a number");
                }

                int index = static_cast<Values::NumberVal*>(propertyVal.get())->value;

                if (index < 0 || index >= (int)array->elements.size()) {
                  ErrorHandler::restart("Array index out of bounds");
                }

                array->elements[index] = evaluate(assignmentExpr->value.get(), env);
                env->assignVar(varname, std::move(array->clone()));
                return array->elements[index]->clone();
              }
            case Values::ValueType::ObjectVal:
              {
                Values::ObjectVal* obj = static_cast<Values::ObjectVal*>(memberVal.get());

                String propertyName;
                if (member->computed) {
                  if (propertyVal->type != Values::ValueType::String) {
                    ErrorHandler::restart("Computed object property must evaluate to a string");
                  }

                  propertyName = static_cast<Values::StringVal*>(propertyVal.get())->str;
                } else {
                  const AstNodes::Identifier* identifier = static_cast<const AstNodes::Identifier*>(member->property.get());
                  propertyName = identifier->symbol;
                }

                if (obj->properties.find(propertyName) == obj->properties.end()) {
                  char errMsg[100];
                  snprintf(errMsg, sizeof(errMsg), "Cannot resolve object property name \"%s\"", propertyName.c_str());
                  ErrorHandler::restart(errMsg);
                }

                obj->properties[propertyName] = evaluate(assignmentExpr->value.get(), env);
                env->assignVar(varname, std::move(obj->clone()));
                return obj->properties[propertyName]->clone();
              }
            default:
              ErrorHandler::restart("Compiler Error (should not happen) Found assignment expression with member access on non-object/non-array value");
              break;
          }
        }
      default:
        ErrorHandler::restart("Expected identifier or member expression on left side of assignment expression");
        return nullptr;
    }
  }
}

std::unique_ptr<Values::ObjectVal> Interpreter::evalObjectExpr(const AstNodes::ObjectLiteral* obj, Environment* env) {
  Serial.println("evalObjectExpr");
  std::unique_ptr<Values::ObjectVal> object = std::make_unique<Values::ObjectVal>();

  for (const auto& [key, value] : obj->properties) {
    object->properties[key] = (value == nullptr) ? std::make_unique<Values::NullVal>() : evaluate(value.get(), env);
  }

  return object;
}

std::unique_ptr<Values::ArrayVal> Interpreter::evalArrayExpr(const AstNodes::ArrayLiteral* array, Environment* env) {
  Serial.println("evalArrayExpr");
  std::unique_ptr<Values::ArrayVal> arrayVal = std::make_unique<Values::ArrayVal>();

  arrayVal->elements.reserve(array->elements.size());

  for (size_t i = 0; i < array->elements.size(); i++) {
    arrayVal->elements.push_back(evaluate(array->elements[i].get(), env));
  }

  return arrayVal;
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalCallExpr(const AstNodes::CallExpr* expr, Environment* env) {
  Serial.println("evalCallExpr");
  std::vector<std::unique_ptr<Values::RuntimeVal>> args;
  if (!expr->args.empty()) {
    args.reserve(expr->args.size());

    for (size_t i = 0; i < expr->args.size(); i++) {
      Serial.println("Found function argument ");
      args.push_back(evaluate(expr->args[i].get(), env));
    }
  } else {
    Serial.println("No function arguments found");
  }

  std::unique_ptr<Values::RuntimeVal> fn = evaluate(expr->caller.get(), env);

  Serial.println("Evaluated callExpr");

  if (fn->type != Values::ValueType::NativeFn) {
    ErrorHandler::restart("Cannot call value that is not a function");
  }

  Values::NativeFnVal* function = static_cast<Values::NativeFnVal*>(fn.get());

  Serial.print("Found function ");
  Serial.println(static_cast<const AstNodes::Identifier*>(expr->caller.get())->symbol);


  std::unique_ptr<Values::RuntimeVal> result = function->call(args, env);
  Serial.println("Called function");
  return result;
}

std::unique_ptr<Values::RuntimeVal> Interpreter::evalMemberExpr(const AstNodes::MemberExpr* member, Environment* env) {
  Serial.println("evalMemberExpr");

  std::unique_ptr<Values::RuntimeVal> memberVal = evaluate(member->object.get(), env);

  if (memberVal->type == Values::ValueType::ObjectVal) {
    Values::ObjectVal* obj = static_cast<Values::ObjectVal*>(memberVal.get());

    String propertyName;
    if (member->computed) {
      std::unique_ptr<Values::RuntimeVal> propertyVal = evaluate(member->property.get(), env);

      if (propertyVal->type != Values::ValueType::String) {
        ErrorHandler::restart("Computed object property must evaluate to a string");
      }

      propertyName = static_cast<Values::StringVal*>(propertyVal.get())->str;
    } else {
      const AstNodes::Identifier* identifier = static_cast<const AstNodes::Identifier*>(member->property.get());
      propertyName = identifier->symbol;
    }

    if (obj->properties.find(propertyName) == obj->properties.end()) {
      char errMsg[100];
      snprintf(errMsg, sizeof(errMsg), "Cannot resolve object property name \"%s\"", propertyName.c_str());
      ErrorHandler::restart(errMsg);
    }

    return obj->properties[propertyName]->clone();
  } else if (memberVal->type == Values::ValueType::ArrayVal) {
    Values::ArrayVal* array = static_cast<Values::ArrayVal*>(memberVal.get());

    if (member->computed) {
      std::unique_ptr<Values::RuntimeVal> propertyVal = evaluate(member->property.get(), env);

      if (propertyVal->type != Values::ValueType::Number) {
        ErrorHandler::restart("Computed property must evaluate to a number");
      }

      int index = static_cast<Values::NumberVal*>(propertyVal.get())->value;

      if (index < 0 || index >= (int)array->elements.size()) {
        ErrorHandler::restart("Array index out of bounds");
      }

      return array->elements[index]->clone();
    } else {
      ErrorHandler::restart("Cannot perform member access with '.' on array value");
    }
  }

  ErrorHandler::restart("Cannot perform member access on non-object/non-array value");
  return memberVal;
}