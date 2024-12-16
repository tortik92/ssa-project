#pragma once

#include <Arduino.h>

#include "interpreter/Values.h"

class ErrorHandler {
public:
  static void printMemoryStats(const char* location) {
    Serial.print("\n-----");
    Serial.print(location);
    Serial.println("-----");
    Serial.print("FreeContStack: ");
    Serial.println(ESP.getFreeContStack());

    uint32_t free;
    uint32_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    Serial.print("Free Heap: ");
    Serial.println(free);
    Serial.print("Max Heap: ");
    Serial.println(max);
    Serial.print("Heap fragmentation: ");
    Serial.print(frag);
    Serial.println("%");
  }

  static void outOfMemoryForNodes(const char* nodeType) {
    Serial.print("Out of memory for ");
    Serial.print(nodeType);
    Serial.println(" nodes");
    actualRestart();
  }

  static void outOfMemoryForRuntimeVal(Values::ValueType valueType) {
    Serial.print("Out of memory for ");
    switch (valueType)
    {
    case Values::ValueType::Null:
      Serial.print("NullVal");
      break;
    case Values::ValueType::Boolean:
      Serial.print("BooleanVal");
      break;
    case Values::ValueType::Number:
      Serial.print("NumberVal");
      break;
    case Values::ValueType::NativeFn:
      Serial.print("NativeFnVal");
      break;
    case Values::ValueType::Break:
      Serial.print("BreakVal");
      break;
    }
    Serial.println(" runtime values");
  }

  static void expectedType(Values::ValueType valueType, const char* location) {
    Serial.println();
    switch (valueType)
    {
    case Values::ValueType::Number:
      /* code */
      break;
    
    default:
      break;
    }
  }

  static void restart(const char* err) {
    Serial.println(err);
    actualRestart();
  }

  static void restart(const char* before, const char* val, const char* after) {
    Serial.print(before);
    Serial.print(val);
    Serial.println(after);
    actualRestart();
  }

  static void restart(const char* expected, const char* actual) {
    Serial.print("Expected \"");
    Serial.print(expected);
    Serial.print("\", got \"");
    Serial.print(actual);
    Serial.println("\"");
    actualRestart();
  }

private:
  static void actualRestart() {
    Serial.println("\nRestarting...");
    delay(5000);
    ESP.restart();
  }
};