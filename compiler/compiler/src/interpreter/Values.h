#pragma once

#include "Constants.h"
#include "GlobalFunctions.h"

class Values {
public:
  enum class ValueType {
    Null,
    Boolean,
    Number
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

  NullVal* newNullVal() {
    if (nullValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for NullVal runtime values");
    }
    return &nullValPool[nullValCount++];
  }

  BooleanVal* newBooleanVal() {
    if (booleanValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for BooleanVal runtime values");
    }
    return &booleanValPool[booleanValCount++];
  }

  NumberVal* newNumberVal() {
    if (numberValCount >= poolSize) {
      GlobalFunctions::restart("Out of memory for NumberVal runtime values");
    }
    return &numberValPool[numberValCount++];
  }

  void clearPools() {
    for(size_t i = 0; i < poolSize; i++) {
      nullValPool[i] = NullVal();
      booleanValPool[i] = BooleanVal();
      numberValPool[i] = NumberVal();
    }

    nullValCount = 0;
    booleanValCount = 0;
    numberValCount = 0;
  }
private:
  NullVal nullValPool[poolSize];
  BooleanVal booleanValPool[poolSize];
  NumberVal numberValPool[poolSize];
  size_t nullValCount = 0;
  size_t booleanValCount = 0;
  size_t numberValCount = 0;
};