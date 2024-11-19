#pragma once

#include "Constants.h"
#include "GlobalFunctions.h"
#include "Values.h"

class Environment {
public:
  Environment()
    : parent(nullptr) {}
  Environment(Environment* _parent)
    : parent(_parent) {
  }

  ~Environment() {
    for(size_t i = 0; i < varIndex; i++) {
      delete[] varNames[i];
    }
  }

  Values::RuntimeVal* declareVar(const char* varName, Values::RuntimeVal* value);
  Values::RuntimeVal* assignVar(const char* varName, Values::RuntimeVal* value);
  Values::RuntimeVal* lookupVar(const char* varName);
  Environment* resolve(const char* varName);

  Values values;
private:
  size_t varIndex = 0;
  Environment* parent;

  char* varNames[maxVariables];
  Values::RuntimeVal* valuesArray[maxVariables];

  bool has(const char* varName);
  void set(const char* varName, Values::RuntimeVal* value);
  Values::RuntimeVal* get(const char* varName);
  int getIndex(const char* varName);
};