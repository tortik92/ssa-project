#pragma once

#include <variant>

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
    volatile bool isOccupied; /**< True if the pad is occupied, false otherwise. */
    bool isActive; /**< True if the pad is active, false otherwise. */
  } Pad;

  /**
   * @enum WaitResult
   * @brief Represents the possible results of a waiting operation.
   */
  enum class WaitResult {
    PadOccupied, /**< A pad has been occupied. */
    UserAbort, /**< The user has sent an abort signal. */
    PadMsgDeliveryError, /**< An error occurred sending the message to the pad. */
    Timeout /**< The waiting operation timed out. */
  };

  /**
   * @brief Plays a single sound on the specified pad.
   * @param soundVal The value representing the sound to be played.
   * @param soundLenMs The duration of the sound in milliseconds.
   * @param padIndex The index of the pad to play the sound on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex = anyPad);
  
  /**
   * @brief Plays 8 sounds on the specified pad.
   * @param soundVal Array of values representing the sounds to be played.
   * @param soundLenMs Array of durations for each sound in milliseconds.
   * @param padIndex The index of the pad to play the sounds on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex = anyPad);

  /**
   * @brief Plays the correct action jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult playCorrectActionJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the wrong action jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult playWrongActionJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the winner jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult playWinnerJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Plays the loser jingle on the specified pad.
   * @param padIndex The index of the pad to play the jingle on (default: anyPad).
   * @return The result of the wait operation (either UserAbort or Timeout).
   */
  WaitResult playLoserJingle(uint8_t padIndex = anyPad);

  /**
   * @brief Waits for a player to occupy a specific pad.
   * @param padIndex The index of the pad to wait for (default: anyPad).
   * @return The result of the wait operation (either PadOccupied, UserAbort or Timeout).
   */
  WaitResult waitForPlayerOnPad(uint8_t padIndex = anyPad);

  /**
   * @brief Waits for a player to occupy any active pad.
   * @return The index of the successfully occupied pad or either of PadOccupied, UserAbort or Timeout.
   */
  std::variant<int, WaitResult> waitForPlayerOnAnyPad();

  /**
   * @brief Waits for players to occupy all active pads.
   * @return The result of the wait operation (either PadOccupied, UserAbort or Timeout).
   */
  WaitResult waitForPlayersOnAllActivePads();

  /**
   * @brief Waits for a specified time and checks if the user sent an abort signal.
   * @param ms The duration to wait in milliseconds.
   * @return True if the user sent an abort signal, false otherwise.
   */
  bool waitWithCancelCheck(unsigned long ms);

  /**
   * @brief Checks if the user has sent an abort signal.
   * @return True if the user has sent an abort signal, false otherwise.
   */
  bool checkForCancelSignal();

  /**
   * @brief Initializes ESP-NOW communication.
   * @param OnDataSent Callback function for when data is sent.
   * @param OnDataRecv Callback function for when data is received.
   */
  void initEspNow(esp_now_send_cb_t OnDataSent, esp_now_recv_cb_t OnDataRecv);

  /**
   * @brief Sends a message to a specific pad.
   * @param padIndex The index of the pad to send the message to. If padIndex is anyPad, the message is broadcasted.
   * @return True if the message was sent successfully, else false.
   */
  bool espNowSendMsg(uint8_t padIndex);

  /**
   * @brief Processes a message received from a pad.
   * @param mac The MAC address of the pad that sent the message.
   * @param incomingData The data received from the pad.
   * @param len The length of the data received.
   */
  void gotMessageFromPad(uint8_t* mac, uint8_t* incomingData);
  
  // helper functions
  void cancelCurrentPadsOperation(uint8_t cancelCode);
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
  SendMsg toSendMsg; /**< Structure holding the data to be sent. */

  Pad padsArray[padsCount]; /**< Array of pads. */

  uint8_t broadcastAddress[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; /**< Broadcast MAC address for communication. */
  uint8_t myMACAddr[6] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 }; /**< MAC address of this device. */
  uint8_t padMACAddrs[padsCount][6] = { /**< Array of MAC addresses for all pads. */
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 }
  };
};