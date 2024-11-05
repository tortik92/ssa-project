#pragma once

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Arduino.h>

#include "BLEComm.h"
#include "Constants.h"

class PadsComm {
public:
  PadsComm(const PadsComm &copy) = delete;

  // structs/enums
  typedef struct RecvMsg {
    uint8_t returnValue;
  } RecvMsg;

  typedef struct SendMsg {
    uint8_t function;
    int param1[paramLen];
    int param2[paramLen];
    int param3[paramLen];
    int param4[paramLen];
  } SendMsg;

  typedef struct Pad {
    uint8_t macAddr[6];
    bool isOccupied;
    bool isActive;
  } Pad;

  enum class PadsState {
    Init,
    Idle,
    WaitingForSpecificPadOccupied,
    WaitingForAnyPadOccupied,
    WaitingForAllActivePadsOccupied,
    Abort
  };

  enum class WaitResult {
    PadOccupied,
    CancelGame,
    Timeout
  };

  static PadsComm* instance;

  WaitResult playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex = UINT8_MAX);
  WaitResult play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex = UINT8_MAX);

  WaitResult playCorrectActionJingle(uint8_t padIndex = UINT8_MAX);
  WaitResult playWrongActionJingle(uint8_t padIndex = UINT8_MAX);
  WaitResult playWinnerJingle(uint8_t padIndex = UINT8_MAX);
  WaitResult playLoserJingle(uint8_t padIndex = UINT8_MAX);

  WaitResult waitForPlayerOnPad(uint8_t padIndex = UINT8_MAX);
  WaitResult waitForPlayerOnAnyPad();
  WaitResult waitForPlayersOnAllActivePads();

  WaitResult waitWithEventChecks(unsigned long ms);

  void initEspNow(esp_now_send_cb_t OnDataSent, esp_now_recv_cb_t OnDataRecv);
  void cancelOperation();
  void prepareSend();
  void prepareWait();
  int findFirstEmptySlot(uint8_t *arr, size_t n);

  // getter/setter
  static PadsComm* getInstance() {
    if(instance == nullptr) {
      instance = new PadsComm();
    }
    return instance;
  }
  
  Pad* getPad(int index) {
    return index < maxAllowedPads && index >= 0 ? &padsArray[index] : nullptr;
  }

  Pad* getPad(uint8_t* mac) {
    for(int i = 0; i < maxAllowedPads; i++) {
      if(memcmp(padsArray[i].macAddr, mac, 6) == 0) {
        return &padsArray[i];
      }
    }
    return nullptr;
  }

  void setPadOccupied(uint8_t* mac, uint8_t* incomingData) {
    switch (currentPadsState) {
      case PadsState::WaitingForAnyPadOccupied:
        anyPadOccupied = true;
        break;
      case PadsState::WaitingForSpecificPadOccupied: {
        Pad* pad = getPad(waitingForSpecificPadOccupied);

        if (pad != nullptr && memcmp((*pad).macAddr, mac, 6) == 0) {
          (*pad).isOccupied = true;
        }
        break;
      }
      case PadsState::WaitingForAllActivePadsOccupied:
        for (int padIndex = 0; padIndex < maxAllowedPads; padIndex++) {
          if (memcmp(padsArray[padIndex].macAddr, mac, 6) == 0) {
            int emptySlot = findFirstEmptySlot(eventOrder, maxAllowedPads);

            if (emptySlot != -1) {
              allPadsOccupied = (emptySlot == maxAllowedPads - 1);
              eventOrder[emptySlot] = padIndex;
              break;
            } else {
              Serial.println("eventOrder array overflow detected");
            }
          }
        }
        break;
      default:
        break;
    }

    memcpy(&toRecvMsg, incomingData, sizeof(toRecvMsg));
  }


  bool getCancelFlag() const { return cancelFlag; }
  void setCancelFlag(bool cancelFlag_) { cancelFlag = cancelFlag_; }
  bool getAllPadsOccupied() const { return allPadsOccupied; }
  void setAllPadsOccupied(bool allPadsOccupied_) { allPadsOccupied = allPadsOccupied_; }
  bool getEventTrackingFlag() const { return eventTrackingFlag; }
  void setEventTrackingFlag(bool eventTrackingFlag_) { eventTrackingFlag = eventTrackingFlag_; }
  bool getAnyPadOccupied() const { return anyPadOccupied; }
  void setAnyPadOccupied(bool anyPadOccupied_) { anyPadOccupied = anyPadOccupied_; }
  PadsState getCurrentPadsState() const { return currentPadsState; }
  uint8_t getWaitingForSpecificPadOccupied() const { return waitingForSpecificPadOccupied; }
  uint8_t getEventOrder(uint8_t index) {
    return index < maxAllowedPads && index >= 0 ? eventOrder[index] : UINT8_MAX;
  }
private:
  PadsComm() {}
  // variables
  BLEComm* btComm = BLEComm::getInstance();

  RecvMsg toRecvMsg;
  SendMsg toSendMsg;

  Pad padsArray[maxAllowedPads];

  // flags
  PadsState currentPadsState = PadsState::Init;

  uint8_t waitingForSpecificPadOccupied = UINT8_MAX;

  uint8_t eventOrder[maxAllowedPads] = { 0 };
  bool eventTrackingFlag = false;
  bool anyPadOccupied = false;
  bool allPadsOccupied = false;
  bool cancelFlag = false;

  // MAC Addresses
  uint8_t broadcastAddress[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t myMACAddr[6] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 };
  uint8_t padMACAddrs[maxAllowedPads][6] = {
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 }/*,
    { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x04 }*/
  };
};