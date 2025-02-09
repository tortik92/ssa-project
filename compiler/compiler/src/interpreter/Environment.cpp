#include "Environment.h"

Values::RuntimeVal Environment::declareVar(const char* varName, const Values::RuntimeVal& value, bool constant) {
  bool insertionSuccessful = variables.insert({ varName, value }).second;

  if (!insertionSuccessful) {
    ErrorHandler::restart("Cannot declare variable \"", varName, "\", as it is already defined");
  }

  if (constant) {
    constants.insert(varName);
  }

  return value;
}

Values::RuntimeVal Environment::assignVar(const char* varName, const Values::RuntimeVal& value) {
  Environment* env = this->resolve(varName);
  if (env->constants.find(varName) != env->constants.end()) {
    ErrorHandler::restart("Trying to reassign const variable");
  }
  env->variables[varName] = value;
  return value;
}

Values::RuntimeVal Environment::lookupVar(const char* varName) {
  Environment* env = resolve(varName);
  return env->variables.at(varName);
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
  declareVar("true", Values::BooleanVal(true), true);
  declareVar("false", Values::BooleanVal(false), true);
  declareVar("null", Values::NullVal(), true);
  Values::NativeFnVal print = Values::NativeFnVal();

  print.call = [](std::vector<Values::RuntimeVal> args, Environment* scope) -> Values::RuntimeVal {
    Serial.println("In Print");
    if (args.size() != 1) {
      ErrorHandler::restart("1 argument expected for function print()");
    }

    switch (args[0].type) {
      case Values::ValueType::Boolean:
        if (static_cast<const Values::BooleanVal*>(&args[0])->value) {
          Serial.println("true");
        } else {
          Serial.println("false");
        }
        break;
      case Values::ValueType::Null:
        Serial.println("null");
        break;
      case Values::ValueType::Number:
        Serial.println(static_cast<const Values::NumberVal*>(&args[0])->value);
        break;
      case Values::ValueType::NativeFn:
        Serial.println((unsigned int)&static_cast<const Values::NativeFnVal*>(&args[0])->call);  // print memory address
        break;
      case Values::ValueType::Break:
        Serial.println("break");
        break;
    }

    return Values::NullVal();
  };

  declareVar("print", print, true);

  Values::NativeFnVal playSound = Values::NativeFnVal();
  playSound.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();

    uint8_t argsLen = 3;
    char argNames[argsLen][9] = { "soundVal", "soundLen", "padIndex" };
    bool required[argsLen] = { true, true, false };
    int parsedArgs[argsLen] = { 0, 0, anyPad };

    for (uint8_t i = 0; i < argsLen; i++) {
      if (i >= args.size()) {
        if (required[i]) {
          ErrorHandler::restart("Required argument '", argNames[i], "' not given for 'playSound(soundVal, soundLen)'");
        } else {
          continue;
        }
      }

      if (args[i].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for '", argNames[i], "' of 'playSound(soundVal, soundLen)'");
      }

      parsedArgs[i] = static_cast<const Values::NumberVal*>(&args[i])->value;
    }

    padsComm->playSingleSound(parsedArgs[0], parsedArgs[1], parsedArgs[2]);

    return Values::BooleanVal(true);
  };
  declareVar("playSound", playSound, true);

  Values::NativeFnVal playCorrectActionJingle = Values::NativeFnVal();
  playCorrectActionJingle.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();
    uint8_t padIndex = anyPad;

    if (!args.empty()) {
      if (args[0].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for 'padIndex' of 'playCorrectActionJingle(padIndex)'");
      }
      padIndex = (uint8_t)static_cast<const Values::NumberVal*>(&args[0])->value;
    }

    padsComm->playCorrectActionJingle(padIndex);

    return Values::BooleanVal(true);
  };
  declareVar("playCorrectActionJingle", playCorrectActionJingle, true);

  Values::NativeFnVal playWrongActionJingle = Values::NativeFnVal();
  playWrongActionJingle.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();
    uint8_t padIndex = anyPad;

    if (!args.empty()) {
      if (args[0].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for 'padIndex' of 'playWrongActionJingle(padIndex)'");
      }
      padIndex = (uint8_t)static_cast<const Values::NumberVal*>(&args[0])->value;
    }

    padsComm->playWrongActionJingle(padIndex);

    return Values::BooleanVal(true);
  };
  declareVar("playWrongActionJingle", playWrongActionJingle, true);

  Values::NativeFnVal playWinnerJingle = Values::NativeFnVal();
  playWinnerJingle.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();
    uint8_t padIndex = anyPad;

    if (!args.empty()) {
      if (args[0].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for 'padIndex' of 'playWinnerJingle(padIndex)'");
      }
      padIndex = (uint8_t)static_cast<const Values::NumberVal*>(&args[0])->value;
    }

    padsComm->playWinnerJingle(padIndex);

    return Values::BooleanVal(true);
  };
  declareVar("playWinnerJingle", playWinnerJingle, true);

  Values::NativeFnVal playLoserJingle = Values::NativeFnVal();
  playLoserJingle.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();
    uint8_t padIndex = anyPad;

    if (!args.empty()) {
      if (args[0].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for 'padIndex' of 'playLoserJingle(padIndex)'");
      }
      padIndex = (uint8_t)static_cast<const Values::NumberVal*>(&args[0])->value;
    }

    padsComm->playLoserJingle(padIndex);

    return Values::BooleanVal(true);
  };
  declareVar("playLoserJingle", playLoserJingle, true);

  Values::NativeFnVal waitForPlayerOnPad = Values::NativeFnVal();
  waitForPlayerOnPad.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();
    uint8_t padIndex = anyPad;

    if (!args.empty()) {
      if (args[0].type != Values::ValueType::Number) {
        ErrorHandler::restart("Expected Number for 'padIndex' of 'waitForPlayerOnPad(padIndex)'");
      }
      padIndex = (uint8_t)static_cast<const Values::NumberVal*>(&args[0])->value;
    }

    padsComm->waitForPlayerOnPad(padIndex);

    return Values::BooleanVal(true);
  };
  declareVar("waitForPlayerOnPad", waitForPlayerOnPad, true);

  Values::NativeFnVal waitForPlayerOnAnyPad = Values::NativeFnVal();
  waitForPlayerOnPad.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();

    if (!args.empty()) {
      ErrorHandler::restart("No parameter expected for 'waitForPlayerOnAnyPad()'");
    }

    padsComm->waitForPlayerOnAnyPad();

    return Values::BooleanVal(true);
  };
  declareVar("waitForPlayerOnAnyPad", waitForPlayerOnAnyPad, true);

  Values::NativeFnVal waitForPlayersOnAllActivePads = Values::NativeFnVal();
  waitForPlayersOnAllActivePads.call = [](std::vector<Values::RuntimeVal> args, Environment* env) -> Values::RuntimeVal {
    PadsComm* padsComm = PadsComm::getInstance();

    if (!args.empty()) {
      ErrorHandler::restart("No parameter expected for 'waitForPlayersOnAllActivePads()'");
    }

    padsComm->waitForPlayersOnAllActivePads();

    return Values::BooleanVal(true);
  };
  declareVar("waitForPlayersOnAllActivePads", waitForPlayersOnAllActivePads, true);
}