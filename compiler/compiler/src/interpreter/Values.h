#pragma once

#include <functional>
#include <array>
#include <forward_list>

#include "Constants.h"
#include "ErrorHandler.h"

class Environment;

class Values {
public:
  ~Values() {
    delete[] nullValPool;
    delete[] booleanValPool;
    delete[] numberValPool;
    delete[] nativeFnPool;
    delete[] breakPool;
  }

  enum class ValueType {
    Null,
    Boolean,
    Number,
    NativeFn,
    Break,
  };

  typedef struct RuntimeVal {
    ValueType type;

    RuntimeVal()
      : type(ValueType::Null) {}
    RuntimeVal(ValueType _type)
      : type(_type) {}
  } RuntimeVal;

  typedef struct NullVal : RuntimeVal {
    NullVal()
      : RuntimeVal(ValueType::Null) {}
  } NullVal;

  typedef struct BooleanVal : RuntimeVal {
    bool value;
    BooleanVal()
      : RuntimeVal(ValueType::Boolean), value(true) {}
    BooleanVal(bool _value)
      : RuntimeVal(ValueType::Boolean), value(_value) {}
  } BooleanVal;

  typedef struct NumberVal : RuntimeVal {
    int value;

    NumberVal()
      : RuntimeVal(ValueType::Number), value(0L) {}
    NumberVal(int _value)
      : RuntimeVal(ValueType::Number), value(_value) {}
  } NumberVal;

  using FunctionCall = std::function<RuntimeVal*(RuntimeVal* args[maxFunctionArgs], Environment* env)>;

  typedef struct NativeFnValue : RuntimeVal {
    FunctionCall call;

    NativeFnValue()
      : RuntimeVal(ValueType::NativeFn) {}
  } NativeFnValue;

  typedef struct BreakVal : RuntimeVal {
    BreakVal()
      : RuntimeVal(ValueType::Break) {}
  } BreakVal;

  template<typename T>
  T* newRuntimeVal(ValueType vt) {
    switch (vt)
    {
    case ValueType::Boolean :
      /* code */
      break;
    
    default:
      break;
    }
  }

  NullVal* newNullVal() {
    if (nullValCount >= poolSize) {
      ErrorHandler::restart("Out of memory for NullVal runtime values");
    }
    return &nullValPool[nullValCount++];
  }

  BooleanVal* newBooleanVal(bool value = true) {
    if (booleanValCount >= poolSize) {
      ErrorHandler::restart("Out of memory for BooleanVal runtime values");
    }
    booleanValPool[booleanValCount] = value;
    return &booleanValPool[booleanValCount++];
  }

  NumberVal* newNumberVal() {
    if (numberValCount >= poolSize) {
      ErrorHandler::restart("Out of memory for NumberVal runtime values");
    }
    return &numberValPool[numberValCount++];
  }

  NativeFnValue* newNativeFn() {
    if (nativeFnCount >= poolSize) {
      ErrorHandler::restart("Out of memory for NativeFnVal runtime values");
    }
    return &nativeFnPool[nativeFnCount++];
  }

  BreakVal* newBreakVal() {
    if (breakCount >= poolSize) {
      ErrorHandler::restart("Out of memory for BreakVal runtime values");
    }
    return &breakPool[breakCount++];
  }


  void clearPools() {
    for (size_t i = 0; i < poolSize; i++) {
      nullValPool[i] = NullVal();
      booleanValPool[i] = BooleanVal();
      numberValPool[i] = NumberVal();
      nativeFnPool[i] = NativeFnValue();
      breakPool[i] = BreakVal();
    }

    nullValCount = 0;
    booleanValCount = 0;
    numberValCount = 0;
    nativeFnCount = 0;
    breakCount = 0;
  }
private:
  NullVal* nullValPool = new NullVal[poolSize];
  BooleanVal* booleanValPool = new BooleanVal[poolSize];
  NumberVal* numberValPool = new NumberVal[poolSize];
  NativeFnValue* nativeFnPool = new NativeFnValue[poolSize];
  BreakVal* breakPool = new BreakVal[poolSize];

  size_t nullValCount = 0;
  size_t booleanValCount = 0;
  size_t numberValCount = 0;
  size_t nativeFnCount = 0;
  size_t breakCount = 0;
};