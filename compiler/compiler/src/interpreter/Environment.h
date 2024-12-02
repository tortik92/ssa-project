#pragma once

#include "Constants.h"
#include "GlobalFunctions.h"
#include "Values.h"
#include "comm/PadsComm.h"

class Environment {
public:
  Environment()
    : parent(nullptr) {
    declareVar("true", values.newBooleanVal(true), true);
    declareVar("false", values.newBooleanVal(false), true);
    declareVar("null", values.newNullVal(), true);
    Values::NativeFnValue* print = values.newNativeFn();


    print->call = [](Values::RuntimeVal** args, Environment* scope) -> Values::RuntimeVal* {
      switch (args[0]->type) {
        case Values::ValueType::Boolean:
          if (static_cast<const Values::BooleanVal*>(args[0])->value) {
            Serial.println("true");
          } else {
            Serial.println("false");
          }
          break;
        case Values::ValueType::Null:
          Serial.println("null");
          break;
        case Values::ValueType::Number:
          Serial.println(static_cast<const Values::NumberVal*>(args[0])->value);
          break;
        case Values::ValueType::NativeFn:
          Serial.println((unsigned int)&static_cast<const Values::NativeFnValue*>(args[0])->call);  // print memory address
          break;
        default:
          GlobalFunctions::restart("Undefined Value Type");
          break;
      }

      return scope->values.newNullVal();
    };

    declareVar("print", print, true);

    Values::NativeFnValue* playSound = values.newNativeFn();
    playSound->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();

      uint8_t argsLen = 3;
      char argNames[argsLen][9] = { "soundVal", "soundLen", "padIndex" };
      bool required[argsLen] = { true, true, false };
      int parsedArgs[argsLen] = {0, 0, UINT8_MAX};

      for (uint8_t i = 0; i < argsLen; i++) {
        if (args[i] == nullptr) {
          if (required[i]) {
            GlobalFunctions::restart("Required argument '", argNames[i], "' not given for 'playSound(soundVal, soundLen)'");
          } else {
            continue;
          }
        } 
        
        if (args[i]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for '", argNames[i], "' of 'playSound(soundVal, soundLen)'");
        }

        parsedArgs[i] = static_cast<const Values::NumberVal*>(args[i])->value;
      }

      padsComm->playSingleSound(parsedArgs[0], parsedArgs[1], parsedArgs[2]);

      return env->values.newBooleanVal(true);
    };
    declareVar("playSound", playSound, true);

    Values::NativeFnValue* playCorrectActionJingle = values.newNativeFn();
    playCorrectActionJingle->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();
      uint8_t padIndex = UINT8_MAX;

      if(args[0] != nullptr) {
        if (args[0]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for 'padIndex' of 'playCorrectActionJingle(padIndex)'");
        }
        padIndex = (uint8_t) static_cast<const Values::NumberVal*>(args[0])->value;
      }

      padsComm->playCorrectActionJingle(padIndex);

      return env->values.newBooleanVal(true);
    };
    declareVar("playCorrectActionJingle", playCorrectActionJingle, true);

    Values::NativeFnValue* playWrongActionJingle = values.newNativeFn();
    playWrongActionJingle->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();
      uint8_t padIndex = UINT8_MAX;

      if(args[0] != nullptr) {
        if (args[0]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for 'padIndex' of 'playWrongActionJingle(padIndex)'");
        }
        padIndex = (uint8_t) static_cast<const Values::NumberVal*>(args[0])->value;
      }

      padsComm->playWrongActionJingle(padIndex);

      return env->values.newBooleanVal(true);
    };
    declareVar("playWrongActionJingle", playWrongActionJingle, true);

    Values::NativeFnValue* playWinnerJingle = values.newNativeFn();
    playWinnerJingle->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();
      uint8_t padIndex = UINT8_MAX;

      if(args[0] != nullptr) {
        if (args[0]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for 'padIndex' of 'playWinnerJingle(padIndex)'");
        }
        padIndex = (uint8_t) static_cast<const Values::NumberVal*>(args[0])->value;
      }

      padsComm->playWinnerJingle(padIndex);

      return env->values.newBooleanVal(true);
    };
    declareVar("playWinnerJingle", playWinnerJingle, true);

    Values::NativeFnValue* playLoserJingle = values.newNativeFn();
    playLoserJingle->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();
      uint8_t padIndex = UINT8_MAX;

      if(args[0] != nullptr) {
        if (args[0]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for 'padIndex' of 'playLoserJingle(padIndex)'");
        }
        padIndex = (uint8_t) static_cast<const Values::NumberVal*>(args[0])->value;
      }

      padsComm->playLoserJingle(padIndex);

      return env->values.newBooleanVal(true);
    };
    declareVar("playLoserJingle", playLoserJingle, true);

    Values::NativeFnValue* waitForPlayerOnPad = values.newNativeFn();
    waitForPlayerOnPad->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();
      uint8_t padIndex = UINT8_MAX;

      if(args[0] != nullptr) {
        if (args[0]->type != Values::ValueType::Number) {
          GlobalFunctions::restart("Expected Number for 'padIndex' of 'waitForPlayerOnPad(padIndex)'");
        }
        padIndex = (uint8_t) static_cast<const Values::NumberVal*>(args[0])->value;
      }

      padsComm->waitForPlayerOnPad(padIndex);

      return env->values.newBooleanVal(true);
    };
    declareVar("waitForPlayerOnPad", waitForPlayerOnPad, true);

    Values::NativeFnValue* waitForPlayerOnAnyPad = values.newNativeFn();
    waitForPlayerOnPad->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();

      if(args[0] != nullptr) {
        GlobalFunctions::restart("No parameter expected for 'waitForPlayerOnAnyPad()'");
      }

      padsComm->waitForPlayerOnAnyPad();

      return env->values.newBooleanVal(true);
    };
    declareVar("waitForPlayerOnAnyPad", waitForPlayerOnAnyPad, true);

    Values::NativeFnValue* waitForPlayersOnAllActivePads = values.newNativeFn();
    waitForPlayersOnAllActivePads->call = [](Values::RuntimeVal** args, Environment* env) -> Values::RuntimeVal* {
      PadsComm* padsComm = PadsComm::getInstance();

      if(args[0] != nullptr) {
        GlobalFunctions::restart("No parameter expected for 'waitForPlayersOnAllActivePads()'");
      }

      padsComm->waitForPlayersOnAllActivePads();

      return env->values.newBooleanVal(true);
    };
    declareVar("waitForPlayersOnAllActivePads", waitForPlayersOnAllActivePads, true);
  }
  Environment(Environment* _parent)
    : parent(_parent) {
  }

  ~Environment() {
    for (size_t i = 0; i < varIndex; i++) {
      delete[] varNames[i];
    }
  }

  Values::RuntimeVal* declareVar(const char* varName, Values::RuntimeVal* value, bool constant);
  Values::RuntimeVal* assignVar(const char* varName, Values::RuntimeVal* value);
  Values::RuntimeVal* lookupVar(const char* varName);
  Environment* resolve(const char* varName);

  Values values;
private:
  size_t varIndex = 0;
  Environment* parent;

  char* varNames[maxVariables];
  Values::RuntimeVal* valuesArray[maxVariables];

  bool isConstant[maxVariables] = { 0 };

  bool has(const char* varName);
  void set(const char* varName, Values::RuntimeVal* value);
  Values::RuntimeVal* get(const char* varName);
  int getIndex(const char* varName);
};