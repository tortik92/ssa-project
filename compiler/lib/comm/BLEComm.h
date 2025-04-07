#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

/**
 * @class BLEComm
 * @brief Handles Bluetooth communication for the central module using SoftwareSerial.
 *
 * This class acts as a wrapper around the SoftwareSerial class to facilitate Bluetooth communication
 * between the central module and external devices. It follows a singleton pattern to ensure a single
 * instance manages all Bluetooth input operations.
 */
class BLEComm {
public:
  /**
   * @brief Deleted copy constructor to prevent copying.
   */
  BLEComm(const BLEComm &copy) = delete;
  
  /**
   * @brief Gets the singleton instance of BLEComm.
   * @return Pointer to the single instance of BLEComm.
   */
  static BLEComm* getInstance() {
    if(instance == nullptr) {
      instance = new BLEComm();
    }
    return instance;
  }

  /**
   * @brief Destroys the singleton instance of BLEComm.
   */
  static void destroyInstance() {
    delete instance;
    instance = nullptr;
  }

  /**
   * @brief Checks if there are unread bytes available from the Bluetooth module.
   * @return True if data is available, false otherwise.
   */
  bool hasUnreadBytes();

  /**
   * @brief Reads a single byte from the Bluetooth module.
   * @return The byte read from the Bluetooth module.
   */
  uint8_t readByte();

  /**
   * @brief Reads a complete command or data string from the Bluetooth module.
   * @return A string containing the received command or data.
   */
  String readCode();

private:
  static BLEComm* instance;  ///< Singleton instance of BLEComm.

  /**
   * @brief Private constructor to initialize the SoftwareSerial communication.
   */
  BLEComm() {
    BTserial.begin(9600);
  }

  SoftwareSerial BTserial = SoftwareSerial(D6, D7);  ///< Bluetooth serial communication (RX - TX).
};