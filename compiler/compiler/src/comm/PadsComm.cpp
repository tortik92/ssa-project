#include "PadsComm.h"

PadsComm* PadsComm::instance = nullptr;

// ----------ABSTRACTED PAD CALL FUNCTIONS-----------
PadsComm::WaitResult PadsComm::waitForPlayerOnPad(uint8_t padIndex) {
  padsArray[padIndex].isOccupied = false;
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  padsState = PadsState::WaitingForSpecificPadOccupied;
  waitingForSpecificPadOccupied = padIndex;

  Serial.println();

  esp_now_send(padsArray[padIndex].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  return waitWithEventChecks(defaultDelay);
}

PadsComm::WaitResult PadsComm::waitForPlayerOnAnyPad() {
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  padsState = PadsState::WaitingForAnyPadOccupied;

  Serial.println("Waiting for player on any pad");

  // send wait signal to all active pads
  for (int i = 0; i < maxAllowedPads; i++) {
    padsArray[i].isOccupied = false;
    if (padsArray[i].isActive) {
      esp_now_send(padsArray[i].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
    }
  }

  return waitWithEventChecks(defaultDelay);
}

PadsComm::WaitResult PadsComm::waitForPlayersOnAllActivePads() {
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  padsState = PadsState::WaitingForAllActivePadsOccupied;

  Serial.println("Waiting for players on all pads");

  // send wait signal to all active pads
  for (int i = 0; i < maxAllowedPads; i++) {
    padsArray[i].isOccupied = false;
    if (padsArray[i].isActive) {
      esp_now_send(padsArray[i].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
    }
  }

  return waitWithEventChecks(defaultDelay);
}

PadsComm::WaitResult PadsComm::playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex) {
  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;
  prepareSend();

  toSendMsg.function = padOutput_playSound8Val;
  toSendMsg.param1[0] = soundVal;
  toSendMsg.param2[0] = soundLenMs;

  printWithMac("Playing single sound on pad", macAddr);
  Serial.println(soundVal);

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  return waitWithEventChecks(soundLenMs);
}

PadsComm::WaitResult PadsComm::play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex) {
  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;

  toSendMsg.function = padOutput_playSound8Val;
  memcpy(toSendMsg.param1, soundVal, paramLen);
  memcpy(toSendMsg.param2, soundLenMs, paramLen);

  printWithMac("Playing 8 sounds on pad", macAddr);
  Serial.print("Sounds: [");
  for (int i = 0; i < paramLen; i++) {
    Serial.print(soundVal[i]);
    if (i < paramLen - 1) Serial.print(", ");
    else Serial.println("]");
  }

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  unsigned long delay = 0;
  for (int i = 0; i < paramLen; i++) {
    delay += soundLenMs[i];
  }

  return waitWithEventChecks(delay);
}

PadsComm::WaitResult PadsComm::playCorrectActionJingle(uint8_t padIndex) {
  return play8Sounds(gameActionTones, gameActionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playWrongActionJingle(uint8_t padIndex) {
  int reverseTones[paramLen] = { 0 };
  for (size_t i = 0; i < paramLen; i++) {
    reverseTones[paramLen - 1 - i] = gameActionTones[i];
  }
  return play8Sounds(reverseTones, gameActionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playWinnerJingle(uint8_t padIndex) {
  return play8Sounds(gameConclusionTones, gameConclusionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playLoserJingle(uint8_t padIndex) {
  int reverseTones[paramLen] = { 0 };
  for (size_t i = 0; i < paramLen; i++) {
    reverseTones[paramLen - 1 - i] = gameConclusionTones[i];
  }
  return play8Sounds(reverseTones, gameConclusionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::waitWithEventChecks(unsigned long ms) {
  unsigned long time_now = millis();

  while (millis() - time_now < ms) {
    if (btComm->hasUnreadBytes()) {
      uint8_t incomingByte = btComm->readByte();

      if (incomingByte == phoneInput_cancel) {
        cancelOperation();
        GlobalFunctions::restart("Received abort signal from phone");
      }
    }

    switch (padsState) {
      case PadsState::WaitingForAnyPadOccupied:
        cancelOperation();
        return PadsComm::WaitResult::PadOccupied;
      case PadsState::WaitingForSpecificPadOccupied:
        if (waitingForSpecificPadOccupied != UINT8_MAX) {
          if (padsArray[waitingForSpecificPadOccupied].isOccupied) {
            cancelOperation();
            return PadsComm::WaitResult::PadOccupied;
          }
        }
        break;
      case PadsState::WaitingForAllActivePadsOccupied:
        if (allPadsOccupied) {
          cancelOperation();
          return PadsComm::WaitResult::PadOccupied;
        }
      default:
        break;
    }
  }

  Serial.println("Timeout");  
  return PadsComm::WaitResult::Timeout;
}

void PadsComm::initEspNow(esp_now_send_cb_t OnDataSent, esp_now_recv_cb_t OnDataRecv) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_macaddr(STATION_IF, &myMACAddr[0]);

  if (esp_now_init() != ERR_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Successfully initialized ESP-NOW");
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // add peers
  uint8_t channel = 1;
  uint8_t *key = NULL;
  uint8_t keylen = 0;

  for (int i = 0; i < maxAllowedPads; i++) {
    memcpy(&padsArray[i].macAddr, &padMACAddrs[i], sizeof(padsArray[i].macAddr));
    padsArray[i].isOccupied = false;
    padsArray[i].isActive = true;

    if (esp_now_add_peer(padMACAddrs[i], ESP_NOW_ROLE_COMBO, channel, key, keylen) != 0) {
      padsArray[i].isActive = false;
    } else {
      printWithMac("Could not add pad ", padMACAddrs[i]);
    }
  }

  Serial.println("Added peers");
  padsState = PadsState::Idle;
}

void PadsComm::prepareSend() {
  // set toSendMsg struct to 0
  toSendMsg.function = 0;

  memset(toSendMsg.param1, 0, sizeof(toSendMsg.param1));
  memset(toSendMsg.param2, 0, sizeof(toSendMsg.param2));
  memset(toSendMsg.param3, 0, sizeof(toSendMsg.param3));
  memset(toSendMsg.param4, 0, sizeof(toSendMsg.param4));

  // set toRecvMsg struct to 255 (is not a real return value)
  toRecvMsg.returnValue = 0xFF;
}

void PadsComm::prepareWait() {
  anyPadOccupied = false;
  allPadsOccupied = false;
  waitingForSpecificPadOccupied = UINT8_MAX;
  memset(eventOrder, 0, sizeof(maxAllowedPads));

  for(int i = 0; i < maxAllowedPads; i++) {
    padsArray[i].isOccupied = false;
  }
}


void PadsComm::cancelOperation() {
  waitingForSpecificPadOccupied = UINT8_MAX;

  prepareSend();
  toSendMsg.function = padOutput_cancelOperation;
  esp_now_send(broadcastAddress, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
}

int PadsComm::findFirstEmptySlot(uint8_t *arr, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (arr[i] == 0)
      return i;
  }
  return -1;
}

void PadsComm::printWithMac(const char *msg, uint8_t *mac) {
  Serial.print(msg);
  Serial.write(' ');
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5)
      Serial.print(":");
    else
      Serial.print("\n");
  }
}