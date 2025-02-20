#pragma once

#include <Arduino.h>

/**
 * @class ErrorHandler
 * @brief A utility class for handling errors and memory diagnostics in an Arduino/ESP-based system.
 */
class ErrorHandler {
public:
  /**
   * @brief Prints memory statistics for debugging purposes.
   * @param location A string indicating where the memory stats are being logged.
   */
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

  /**
   * @brief Handles out-of-memory errors for a specific node type.
   * @param nodeType The type of node that ran out of memory.
   */
  static void outOfMemoryForNodes(const char* nodeType) {
    Serial.print("Out of memory for ");
    Serial.print(nodeType);
    Serial.println(" nodes");
    actualRestart();
  }

  /**
   * @brief Restarts the system with an error message.
   * @param err The error message to be printed before restarting.
   */
  static void restart(const char* err) {
    Serial.println(err);
    actualRestart();
  }

  /**
   * @brief Restarts the system with a formatted message.
   * @param before Text to print before the value.
   * @param val The main value to be displayed.
   * @param after Text to print after the value.
   */
  static void restart(const char* before, const char* val, const char* after) {
    Serial.print(before);
    Serial.print(val);
    Serial.println(after);
    actualRestart();
  }

  /**
   * @brief Restarts the system with a formatted message.
   * @param valueType1 The first main value to be displayed.
   * @param valueType2 The second main value to be displayed.
   */
  static void noComparisonPossible(const char* valueType1, const char* valueType2) {
    Serial.print("Cannot compare ");
    Serial.print(valueType1);
    Serial.print(" with ");
    Serial.println(valueType2);
    actualRestart();
  }

  /**
   * @brief Restarts the system with an expected vs actual value message.
   * @param expected The expected value.
   * @param actual The actual value that was received.
   */
  static void restart(const char* expected, const char* actual) {
    Serial.print("Expected \"");
    Serial.print(expected);
    Serial.print("\", got \"");
    Serial.print(actual);
    Serial.println("\"");
    actualRestart();
  }

  static void missingToken(const char* expectedVal) {
    Serial.print("Missing \"");
    Serial.print(expectedVal);
    Serial.println("\"");
    actualRestart();
  }

private:
  /**
   * @brief Performs the actual system restart after a delay.
   */
  static void actualRestart() {
    Serial.println("\nRestarting...");
    delay(5000);
    ESP.restart();
  }
};