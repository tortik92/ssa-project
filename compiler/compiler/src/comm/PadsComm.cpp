#include "PadsComm.h"

PadsComm *PadsComm::instance = nullptr;

PadsComm::WaitResult PadsComm::playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex) {
  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;
  prepareSend();

  toSendMsg.function = padOutput_playSound8Val;
  toSendMsg.param1[0] = soundVal;
  toSendMsg.param2[0] = soundLenMs;

  printWithMac("Playing single sound on pad", macAddr);
  Serial.println(soundVal);

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  return waitWithEventChecks(soundLenMs + mandatorySendDelay);
}

PadsComm::WaitResult PadsComm::play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex) {
  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;
  toSendMsg.function = padOutput_playSound8Val;
  memcpy(toSendMsg.param1, soundVal, sizeof(toSendMsg.param1));
  memcpy(toSendMsg.param2, soundLenMs, sizeof(toSendMsg.param2));

  printWithMac("Playing 8 sounds on pad", macAddr);
  Serial.print("Sounds: [");
  for (int i = 0; i < paramLen; i++) {
    Serial.print(toSendMsg.param1[i]);
    if (i < paramLen - 1) Serial.print(", ");
    else Serial.println("]");
  }

  Serial.print("Lengths: [");
  for (int i = 0; i < paramLen; i++) {
    Serial.print(toSendMsg.param2[i]);
    if (i < paramLen - 1) Serial.print(", ");
    else Serial.println("]");
  }

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  unsigned long delay = mandatorySendDelay;
  for (int i = 0; i < paramLen; i++) {
    delay += soundLenMs[i];
  }

  return waitWithEventChecks(delay);
}

PadsComm::WaitResult PadsComm::playCorrectActionJingle(uint8_t padIndex) {
  return play8Sounds(correctActionTones, correctActionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playWrongActionJingle(uint8_t padIndex) {
  return play8Sounds(wrongActionTones, wrongActionDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playWinnerJingle(uint8_t padIndex) {
  return play8Sounds(winnerTones, winnerToneDurations, padIndex);
}

PadsComm::WaitResult PadsComm::playLoserJingle(uint8_t padIndex) {
  return play8Sounds(loserTones, loserToneDurations, padIndex);
}

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

PadsComm::WaitResult PadsComm::waitWithEventChecks(unsigned long ms) {
  unsigned long time_now = millis();

  while (millis() - time_now < ms) {
    if (btComm->hasUnreadBytes()) {
      uint8_t incomingByte = btComm->readByte();

      if (incomingByte == phoneInput_cancel) {
        cancelOperation();
        ErrorHandler::restart("Received abort signal from phone");
      }
    }

    switch (padsState) {
      case PadsState::AnyPadOccupied:
        Serial.println("Any pad occupied!");
        cancelOperation();
        padsState = PadsState::Idle;
        Serial.println("Exiting waitWithEventChecks early...");
        return WaitResult::PadOccupied;
      case PadsState::SpecificPadOccupied:
        Serial.println("Specific pad occupied!");
        cancelOperation();
        padsState = PadsState::Idle;
        Serial.println("Exiting waitWithEventChecks early...");
        return WaitResult::PadOccupied;
      case PadsState::AllActivePadsOccupied:
        Serial.println("All active pads occupied!");
        cancelOperation();
        padsState = PadsState::Idle;
        Serial.println("Exiting waitWithEventChecks early...");
        return WaitResult::PadOccupied;
      default:
        break;
    }
  }

  Serial.print("Finished waiting ");
  Serial.print(ms);
  Serial.println(" ms");
  return WaitResult::Timeout;
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

void PadsComm::cancelOperation() {
  waitingForSpecificPadOccupied = anyPad;

  prepareSend();
  toSendMsg.function = padOutput_cancelOperation;
  esp_now_send(broadcastAddress, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
  delay(500);
}

void PadsComm::prepareSend() {
  memset(&toSendMsg, 0, sizeof(toSendMsg));

  // set toRecvMsg struct to 255 (is not a real return value)
  toRecvMsg.returnValue = 0xFF;
}

void PadsComm::prepareWait() {
  waitingForSpecificPadOccupied = anyPad;

  for (int i = 0; i < maxAllowedPads; i++) {
    padsArray[i].isOccupied = false;
    padOccupationChronology[i] = 0;
  }
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

void PadsComm::setPadOccupied(uint8_t *mac, uint8_t *incomingData) {
  switch (padsState) {
    case PadsState::WaitingForAnyPadOccupied:
      {
        padsState = PadsState::AnyPadOccupied;

        Pad *pad = findPad(mac);
        if (pad != nullptr) {
          pad->isOccupied = true;
        }

        break;
      }
    case PadsState::WaitingForSpecificPadOccupied:
      {
        Pad *pad = findPad(waitingForSpecificPadOccupied);

        if (pad != nullptr && memcmp(pad->macAddr, mac, 6) == 0) {
          pad->isOccupied = true;
          padsState = PadsState::SpecificPadOccupied;
        }
        break;
      }
    case PadsState::WaitingForAllActivePadsOccupied:
      for (int padIndex = 0; padIndex < maxAllowedPads; padIndex++) {
        if (memcmp(padsArray[padIndex].macAddr, mac, 6) == 0) {
          int emptyIndex = findFirstEmptySlot(padOccupationChronology, maxAllowedPads);

          if (emptyIndex != -1) {
            if (emptyIndex == maxAllowedPads - 1) {
              padsState = PadsState::AllActivePadsOccupied;
            }
            padOccupationChronology[emptyIndex] = padIndex;
            break;
          } else {
            ErrorHandler::restart("padOccupationChronology array overflow detected");
          }
        }
      }
      break;
    default:
      break;
  }

  memcpy(&toRecvMsg, incomingData, sizeof(toRecvMsg));
}

bool PadsComm::isPadOccupied(uint8_t padIndex) const {
  return padIndex < maxAllowedPads ? padsArray[padIndex].isOccupied : false;
}

PadsComm::Pad *PadsComm::findPad(uint8_t index) {
  return index < maxAllowedPads && index >= 0 ? &padsArray[index] : nullptr;
}

PadsComm::Pad *PadsComm::findPad(uint8_t *mac) {
  for (int i = 0; i < maxAllowedPads; i++) {
    if (memcmp(padsArray[i].macAddr, mac, 6) == 0) {
      return &padsArray[i];
    }
  }
  return nullptr;
}