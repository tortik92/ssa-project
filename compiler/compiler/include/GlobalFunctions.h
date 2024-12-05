#pragma once

#include <Arduino.h>

class GlobalFunctions {
public:
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