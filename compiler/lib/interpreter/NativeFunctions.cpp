#include "NativeFunctions.h"

std::unique_ptr<Values::RuntimeVal> NativeFunctions::print(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  Serial.println("In Print");
  if (args.size() != 1) {
    Serial.print("args size: ");
    Serial.println(args.size());
    ErrorHandler::restart("Too many arguments in function call 'print()'");
  }

  switch (args[0]->type) {
    case Values::ValueType::Boolean:
      if (static_cast<Values::BooleanVal*>(args[0].get())->value) {
        Serial.println("true");
      } else {
        Serial.println("false");
      }
      break;
    case Values::ValueType::Null:
      Serial.println("null");
      break;
    case Values::ValueType::Number:
      Serial.println(static_cast<Values::NumberVal*>(args[0].get())->value);
      break;
    case Values::ValueType::String:
      Serial.println(static_cast<Values::StringVal*>(args[0].get())->str);
      break;
    case Values::ValueType::ObjectVal:
      {
        Serial.println("{");

        const std::map<String, std::unique_ptr<Values::RuntimeVal>>& properties = static_cast<Values::ObjectVal*>(args[0].get())->properties;

        for (const auto& [key, value] : properties) {
          Serial.print(key);
          Serial.print(": ");

          // recursive call to print
          std::vector<std::unique_ptr<Values::RuntimeVal>> callArgs;
          callArgs.push_back(value->clone());
          print(callArgs, scope);

          if (key != properties.rbegin()->first) {
            Serial.println(",");
          }
        }

        Serial.println("}");
        break;
      }
    case Values::ValueType::ArrayVal:
      {
        Serial.println("[");

        const std::vector<std::unique_ptr<Values::RuntimeVal>>& elements = static_cast<Values::ArrayVal*>(args[0].get())->elements;

        for (size_t i = 0; i < elements.size(); i++) {
          // recursive call to print
          std::vector<std::unique_ptr<Values::RuntimeVal>> callArgs;
          callArgs.push_back(elements[i]->clone());
          print(callArgs, scope);

          if (i != elements.size() - 1) {
            Serial.println(",");
          }
        }

        Serial.println("]");
        break;
      }
    case Values::ValueType::NativeFn:
      Serial.println((unsigned int)&static_cast<const Values::NativeFnVal*>(args[0].get())->call);  // print memory address
      break;
    case Values::ValueType::Break:
      Serial.println("break");
      break;
  }

  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::rnd(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  int returnValue = 0;

  switch (args.size())
  {
  case 0:
    ErrorHandler::restart("Required argument 'max' not given for 'random(max)'");
    break;
  case 1:
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'max' of 'random(max)'");
    }
    returnValue = random(static_cast<const Values::NumberVal*>(args[0].get())->value);
    break;
  case 2:
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'min' of 'random(min, max)'");
    } else if (args[1]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'max' of 'random(min, max)'");
    }
    returnValue = random(static_cast<const Values::NumberVal*>(args[0].get())->value, static_cast<const Values::NumberVal*>(args[1].get())->value);
    break;
  default:
    ErrorHandler::restart("Too many arguments given for 'random(min, max)'");
    break;
  }

  return std::make_unique<Values::NumberVal>(returnValue);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::playSound(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
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

    if (args[i]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for '", argNames[i], "' of 'playSound(soundVal, soundLen)'");
    }

    parsedArgs[i] = static_cast<const Values::NumberVal*>(args[i].get())->value;
  }

  padsComm->playSingleSound(parsedArgs[0], parsedArgs[1], parsedArgs[2]);

  return std::make_unique<Values::NullVal>();
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::playCorrectActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();
  uint8_t padIndex = anyPad;

  if (!args.empty()) {
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'padIndex' of 'playCorrectActionJingle(padIndex)'");
    }
    padIndex = (uint8_t)static_cast<const Values::NumberVal*>(args[0].get())->value;
  }

  padsComm->playCorrectActionJingle(padIndex);

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::playWrongActionJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();
  uint8_t padIndex = anyPad;

  if (!args.empty()) {
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'padIndex' of 'playWrongActionJingle(padIndex)'");
    }
    padIndex = (uint8_t)static_cast<const Values::NumberVal*>(args[0].get())->value;
  }

  padsComm->playWrongActionJingle(padIndex);

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::playWinnerJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();
  uint8_t padIndex = anyPad;

  if (!args.empty()) {
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'padIndex' of 'playWinnerJingle(padIndex)'");
    }
    padIndex = (uint8_t)static_cast<const Values::NumberVal*>(args[0].get())->value;
  }

  padsComm->playWinnerJingle(padIndex);

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::playLoserJingle(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();
  uint8_t padIndex = anyPad;

  if (!args.empty()) {
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'padIndex' of 'playLoserJingle(padIndex)'");
    }
    padIndex = (uint8_t)static_cast<const Values::NumberVal*>(args[0].get())->value;
  }

  padsComm->playLoserJingle(padIndex);

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::waitForPlayerOnPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();
  uint8_t padIndex = anyPad;

  if (!args.empty()) {
    if (args[0]->type != Values::ValueType::Number) {
      ErrorHandler::restart("Expected Number for 'padIndex' of 'waitForPlayerOnPad(padIndex)'");
    }
    padIndex = (uint8_t)static_cast<const Values::NumberVal*>(args[0].get())->value;
  }

  padsComm->waitForPlayerOnPad(padIndex);

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::waitForPlayerOnAnyPad(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();

  if (!args.empty()) {
    ErrorHandler::restart("No parameter expected for 'waitForPlayerOnAnyPad()'");
  }

  std::variant<int, PadsComm::WaitResult> result = padsComm->waitForPlayerOnAnyPad();

  if(std::holds_alternative<PadsComm::WaitResult>(result)) {
    return std::make_unique<Values::NumberVal>(-1);
  } else {
    return std::make_unique<Values::NumberVal>(std::get<int>(result));
  }
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::waitForPlayersOnAllActivePads(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();

  if (!args.empty()) {
    ErrorHandler::restart("No parameter expected for 'waitForPlayersOnAllActivePads()'");
  }

  padsComm->waitForPlayersOnAllActivePads();

  return std::make_unique<Values::BooleanVal>(true);
}

std::unique_ptr<Values::RuntimeVal> NativeFunctions::waitWithCancelCheck(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();

  if (args.size() != 1) {
    ErrorHandler::restart("1 parameter expected for 'waitWithCancelCheck(delay)'");
  } else if (args[0]->type != Values::ValueType::Number) {
    ErrorHandler::restart("Expected Number for 'waitWithCancelCheck(delay)'");
  }

  bool userAborted = padsComm->waitWithCancelCheck(static_cast<Values::NumberVal*>(args[0].get())->value);

  return std::make_unique<Values::BooleanVal>(userAborted);
}


std::unique_ptr<Values::RuntimeVal> NativeFunctions::isPadOccupied(std::vector<std::unique_ptr<Values::RuntimeVal>>& args, Environment* scope) {
  PadsComm* padsComm = PadsComm::getInstance();

  if (args.size() != 1) {
    ErrorHandler::restart("Parameter expected for 'isPadOccupied(padIndex)'");
  } else if (args[0]->type != Values::ValueType::Number) {
    ErrorHandler::restart("Expected Number for 'isPadOccupied(padIndex)'");
  }

  int padIndex = static_cast<Values::NumberVal*>(args[0].get())->value;

  return std::make_unique<Values::BooleanVal>(padsComm->isPadOccupied(padIndex));
}