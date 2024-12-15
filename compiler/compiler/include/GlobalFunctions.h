#pragma once

#include <Arduino.h>

class GlobalFunctions {
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