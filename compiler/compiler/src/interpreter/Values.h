#pragma once

#include <functional>

#include "Constants.h"
#include "GlobalFunctions.h"

class Environment;

class Values {
public:
  ~Values() {
    Serial.println("Values destructor");
    delete[] nullValPool; 
    delete[] booleanValPool;
    delete[] numberValPool;
    delete[] nativeFnPool;
  }

  enum class ValueType {
    Null,
    Boolean,
    Number,
    NativeFn
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

  typedef struct NativeFnValue : Values::RuntimeVal {
    FunctionCall call;

    NativeFnValue()
      : RuntimeVal(Values::ValueType::NativeFn) {}
  } NativeFnValue;



  NullVal* newNullVal() {
    if (nullValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for NullVal runtime values");
    }
    return &nullValPool[nullValCount++];
  }

  BooleanVal* newBooleanVal(bool value = true) {
    if (booleanValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for BooleanVal runtime values");
    }
    booleanValPool[booleanValCount] = value;
    return &booleanValPool[booleanValCount++];
  }

  NumberVal* newNumberVal() {
    if (numberValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for NumberVal runtime values");
    }
    return &numberValPool[numberValCount++];
  }

  NativeFnValue* newNativeFn() {
    if (nativeFnCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for NumberVal runtime values");
    }
    return &nativeFnPool[nativeFnCount++];
  }


  void clearPools() {
    for (size_t i = 0; i < poolSize; i++) {
      nullValPool[i] = NullVal();
      booleanValPool[i] = BooleanVal();
      numberValPool[i] = NumberVal();
      nativeFnPool[i] = NativeFnValue();
    }

    nullValCount = 0;
    booleanValCount = 0;
    numberValCount = 0;
    nativeFnCount = 0;
  }
private:
  NullVal* nullValPool = new NullVal[poolSize];
  BooleanVal* booleanValPool = new BooleanVal[poolSize];
  NumberVal* numberValPool = new NumberVal[poolSize];
  NativeFnValue* nativeFnPool = new NativeFnValue[poolSize];

  size_t nullValCount = 0;
  size_t booleanValCount = 0;
  size_t numberValCount = 0;
  size_t nativeFnCount = 0;
};