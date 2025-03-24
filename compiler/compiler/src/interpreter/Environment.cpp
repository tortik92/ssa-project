#include "Environment.h"

std::unique_ptr<Values::RuntimeVal> Environment::declareVar(const char* varName, std::unique_ptr<Values::RuntimeVal>&& value, bool constant) {
  auto [it, insertionSuccessful] = variables.insert({ varName, value->clone() });

  if (!insertionSuccessful) {
    ErrorHandler::restart("Cannot declare variable \"", varName, "\", as it is already defined");
  }

  if (constant) {
    constants.insert(varName);
  }

  return std::move(value);
}

std::unique_ptr<Values::RuntimeVal> Environment::assignVar(const char* varName, std::unique_ptr<Values::RuntimeVal>&& value) {
  Environment* env = this->resolve(varName);
  if (env->constants.find(varName) != env->constants.end()) {
    ErrorHandler::restart("Trying to reassign const variable");
  }
  env->variables[varName] = value->clone();
  return std::move(value);
}

std::unique_ptr<Values::RuntimeVal> Environment::lookupVar(const char* varName) {
  Environment* env = resolve(varName);
  return env->variables[varName]->clone();
}

Environment* Environment::resolve(const char* varName) {
  if (variables.find(varName) != variables.end()) {
    return this;
  }

  if (parent == nullptr) {
    ErrorHandler::restart("Cannot resolve variable \"", varName, "\"");
  }

  return parent->resolve(varName);
}

void Environment::createGlobalEnv() {
  declareVar("true", std::make_unique<Values::BooleanVal>(true), true);
  declareVar("false", std::make_unique<Values::BooleanVal>(false), true);
  declareVar("null", std::make_unique<Values::NullVal>(), true);

  declareVar("print", std::make_unique<Values::NativeFnVal>(NativeFunctions::print), true);
  declareVar("random", std::make_unique<Values::NativeFnVal>(NativeFunctions::rnd), true);

  declareVar("playSound", std::make_unique<Values::NativeFnVal>(NativeFunctions::playSound), true);
  declareVar("playCorrectActionJingle", std::make_unique<Values::NativeFnVal>(NativeFunctions::playCorrectActionJingle), true);
  declareVar("playWrongActionJingle", std::make_unique<Values::NativeFnVal>(NativeFunctions::playWrongActionJingle), true);
  declareVar("playWinnerJingle", std::make_unique<Values::NativeFnVal>(NativeFunctions::playWinnerJingle), true);
  declareVar("playLoserJingle", std::make_unique<Values::NativeFnVal>(NativeFunctions::playLoserJingle), true);

  declareVar("waitForPlayerOnPad", std::make_unique<Values::NativeFnVal>(NativeFunctions::waitForPlayerOnPad), true);
  declareVar("waitForPlayerOnAnyPad", std::make_unique<Values::NativeFnVal>(NativeFunctions::waitForPlayerOnAnyPad), true);
  declareVar("waitForPlayersOnAllActivePads", std::make_unique<Values::NativeFnVal>(NativeFunctions::waitForPlayersOnAllActivePads), true);

  declareVar("delay", std::make_unique<Values::NativeFnVal>(NativeFunctions::waitWithCancelCheck), true);

  declareVar("isPadOccupied", std::make_unique<Values::NativeFnVal>(NativeFunctions::isPadOccupied), true);
}