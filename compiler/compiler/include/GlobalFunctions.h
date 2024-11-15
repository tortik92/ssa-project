#pragma once

#include <Arduino.h>

class GlobalFunctions {
public:
  static void restart(const char* err) {
    Serial.println(err);
    Serial.println("\nRestarting...");
    delay(5000);
    ESP.restart();
  }

  static void restart(const char* before, const char* val, const char* after) {
    Serial.print(before);
    Serial.print(val);
    Serial.println(after);
    Serial.println("\nRestarting...");
    delay(5000);
    ESP.restart();
  }
};