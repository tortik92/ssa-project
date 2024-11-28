#pragma once

#include "Constants.h"
#include "GlobalFunctions.h"
#include "Values.h"

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
          Serial.print("Boolean: ");
          Serial.println(static_cast<const Values::BooleanVal*>(args[0])->value);
          break;
        case Values::ValueType::Null:
          Serial.println("Null");
          break;
        case Values::ValueType::Number:
          Serial.print("Number: ");
          Serial.println(static_cast<const Values::NumberVal*>(args[0])->value);
          break;
        case Values::ValueType::NativeFn:
          Serial.println("NativeFn");
          break;
        default:
          GlobalFunctions::restart("Undefined Value Type");
          break;
      }

      return nullptr;
    };

    declareVar("print", print, true);
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