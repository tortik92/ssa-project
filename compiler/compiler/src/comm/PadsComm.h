#pragma once

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>

#include "BLEComm.h"
#include "ErrorHandler.h"
#include "Constants.h"
/**
 * @class PadsComm
 * @brief Manages communication between the central module and smart sport pads using ESP-NOW.
 *
 * This class implements a singleton pattern to handle interactions with multiple sport pads,
 * enabling wireless communication, game coordination, and sensor data retrieval.
 */
class PadsComm {
public:
  /**
   * @brief Delete the copy constructor and assignment operator.
   */
  PadsComm(const PadsComm&) = delete;
  PadsComm& operator=(const PadsComm&) = delete;

  /**
   * @brief Gets the singleton instance of PadsComm.
   * @return Pointer to the single instance of PadsComm.
   */
  static PadsComm* getInstance() {
    if (instance == nullptr) {
      instance = new PadsComm();
    }
    return instance;
  }

  /**
   * @brief Destroys the singleton instance of PadsComm.
   */
  static void destroyInstance() {
    delete instance;
    instance = nullptr;
  }

  // Structs and Enums

  /**
   * @struct RecvMsg
   * @brief Structure representing received messages from the pads.
   */
  typedef struct RecvMsg {
    uint8_t returnValue; /**< The return value from the pad. */
  } RecvMsg;

  /**
   * @struct SendMsg
   * @brief Structure representing messages sent to the pads.
   */
  typedef struct SendMsg {
    uint8_t function; /**< Function to be performed on the pad. */
    int param1[paramLen]; /**< First parameter array for the function. */
    int param2[paramLen]; /**< Second parameter array for the function. */
    int param3[paramLen]; /**< Third parameter array for the function. */
    int param4[paramLen]; /**< Fourth parameter array for the function. */
  } SendMsg;

  /**
   * @struct Pad
   * @brief Represents a single pad's state and unique MAC address.
   */
  typedef struct Pad {
    uint8_t macAddr[6]; /**< MAC address of the pad. */
    bool isOccupied; /**< True if the pad is occupied, false otherwise. */
    bool isActive; /**< True if the pad is active, false otherwise. */
  } Pad;

  /**
   * @enum PadsState
   * @brief Represents different states in the pad communication process.
   */
  enum class PadsState {
    Init, /**< Initial state. */
    Idle, /**< Pads are idle and not in use. */
    WaitingForSpecificPadOccupied, /**< Waiting for a specific pad to be occupied. */
    SpecificPadOccupied, /**< Specific pad has been occupied. */
    WaitingForAnyPadOccupied, /**< Waiting for any pad to be occupied. */
    AnyPadOccupied, /**< Any pad has been occupied. */
    WaitingForAllActivePadsOccupied, /**< Waiting for all active pads to be occupied. */
    AllActivePadsOccupied, /**< All active pads have been occupied. */
    Abort /**< Operation has been aborted. */
  };

  /**
   * @enum WaitResult
   * @brief Represents the possible results of a waiting operation.
   */
  enum class WaitResult {
    PadOccupied, /**< A pad has been occupied. */
    Timeout /**< The waiting operation timed out. */
  };

  /**
   * @brief Plays a single sound on the specified pad.
   * @param soundVal The value representing the sound to be played.
   * @param soundLenMs The duration of the sound in milliseconds.
   * @param padIndex The index of the pad to play the sound on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex = anyPad);
  
  /**
   * @brief Plays 8 sounds on the specified pad.
   * @param soundVal Array of values representing the sounds to be played.
   * @param soundLenMs Array of durations for each sound in milliseconds.
   * @param padIndex The index of the pad to play the sounds on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex = anyPad);

  /**
   * @brief Plays the correct action jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult playCorrectActionJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the wrong action jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult playWrongActionJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the winner jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult playWinnerJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the loser jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult playLoserJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Waits for a player to occupy a specific pad.
   * @param padIndex The index of the pad to wait for (default: anyPad).
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult waitForPlayerOnPad(uint8_t padIndex = anyPad);

  /**
   * @brief Waits for a player to occupy any active pad.
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult waitForPlayerOnAnyPad();

  /**
   * @brief Waits for players to occupy all active pads.
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult waitForPlayersOnAllActivePads();

  /**
   * @brief Waits for an event to occur within a specified time, with checks for certain events.
   * @param ms The maximum duration to wait in milliseconds.
   * @return The result of the wait operation (either PadOccupied or Timeout).
   */
  WaitResult waitWithEventChecks(unsigned long ms);

  /**
   * @brief Initializes ESP-NOW communication.
   * @param OnDataSent Callback function for when data is sent.
   * @param OnDataRecv Callback function for when data is received.
   */
  void initEspNow(esp_now_send_cb_t OnDataSent, esp_now_recv_cb_t OnDataRecv);

  /**
   * @brief Cancels the current operation.
   */
  void cancelOperation();

  /**
   * @brief Prepares the system to send data.
   */
  void prepareSend();

  /**
   * @brief Prepares the system to wait for incoming data.
   */
  void prepareWait();

  /**
   * @brief Finds the first empty slot in an array.
   * @param arr The array to search.
   * @param n The number of elements in the array.
   * @return The index of the first empty slot, or -1 if none found.
   */
  int findFirstEmptySlot(uint8_t* arr, size_t n);

  /**
   * @brief Prints a message along with the MAC address in a readable format.
   * @param msg The message to print.
   * @param mac The MAC address to print.
   */
  void printWithMac(const char* msg, uint8_t* mac);

  /**
   * @brief Sets a pad's occupied state based on incoming data.
   * @param mac The MAC address of the pad.
   * @param incomingData The received message data.
   */
  void setPadOccupied(uint8_t* mac, uint8_t* incomingData);

  /**
   * @brief Checks if a pad is occupied.
   * @param padIndex The index of the pad.
   * @return True if the pad is occupied, false otherwise.
   */
  bool isPadOccupied(uint8_t padIndex) const;
private:
  /**
   * @brief Singleton instance of PadsComm.
   */
  static PadsComm* instance;

  /**
   * @brief Private constructor to enforce singleton pattern.
   */
  PadsComm() {}

  // Variables
  BLEComm* btComm = BLEComm::getInstance(); /**< Bluetooth communication instance. */
  RecvMsg toRecvMsg; /**< Structure holding the received message data. */
  SendMsg toSendMsg; /**< Structure holding the data to be sent. */
  Pad padsArray[maxAllowedPads]; /**< Array of pads, with a size defined by maxAllowedPads. */
  PadsState padsState = PadsState::Init; /**< Current state of the pads communication process. */
  uint8_t waitingForSpecificPadOccupied = UINT8_MAX; /**< Index of the pad being waited for to be occupied. */
  uint8_t padOccupationChronology[maxAllowedPads] = { 0 }; /**< Tracks in which order the pads were occupied. */
  uint8_t broadcastAddress[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; /**< Broadcast MAC address for communication. */
  uint8_t myMACAddr[6] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 }; /**< MAC address of this device. */
  uint8_t padMACAddrs[maxAllowedPads][6] = { /**< Array of MAC addresses for all pads. */
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 }
  };

  // Methods
  Pad* findPad(uint8_t index);
  Pad* findPad(uint8_t* mac);
};