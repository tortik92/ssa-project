#include "PadsComm.h"

PadsComm *PadsComm::instance = nullptr;

PadsComm::WaitResult PadsComm::playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex) {
  memset(&toSendMsg, 0, sizeof(toSendMsg));
  toSendMsg.function = padOutput_playSound8Val;
  toSendMsg.param1[0] = soundVal;
  toSendMsg.param2[0] = soundLenMs;

  Serial.print("Playing single sound ");
  Serial.println(soundVal);

  if (!espNowSendMsg(padIndex)) {
    return WaitResult::PadMsgDeliveryError;
  }

  if(waitWithCancelCheck(soundLenMs)) {
    cancelCurrentPadsOperation(padOutput_cancelPlaySound);
  }

  return waitWithCancelCheck(soundLenMs) ? WaitResult::UserAbort : WaitResult::Timeout;
}

PadsComm::WaitResult PadsComm::play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex) {
  memset(&toSendMsg, 0, sizeof(toSendMsg));
  toSendMsg.function = padOutput_playSound8Val;
  memcpy(toSendMsg.param1, soundVal, sizeof(toSendMsg.param1));
  memcpy(toSendMsg.param2, soundLenMs, sizeof(toSendMsg.param2));

  Serial.print("Play 8 sounds: [");
  for (int i = 0; i < 8; i++) {
    if(soundLenMs[i] == 0) {
      break;
    }
    Serial.print(soundVal[i]);
    if (i < 7 && soundLenMs[i + 1] != 0) Serial.print(", ");
  }
  Serial.print("] | Lengths: [");
  for (int i = 0; i < 8; i++) {
    if(soundLenMs[i] == 0) {
      break;
    }
    Serial.print(soundLenMs[i]);
    if (i < 7 && soundLenMs[i + 1] != 0) Serial.print(", ");
  }
  Serial.println("]");

  if (!espNowSendMsg(padIndex)) {
    return WaitResult::PadMsgDeliveryError;
  }

  unsigned long delay = 0;
  for (int i = 0; i < paramLen; i++) {
    delay += soundLenMs[i];
  }
  
  return waitWithCancelCheck(delay) ? WaitResult::UserAbort : WaitResult::Timeout;
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
  memset(&toSendMsg, 0, sizeof(toSendMsg));
  toSendMsg.function = padOutput_waitForPlayerOnPad;
  toSendMsg.param1[0] = defaultDelay;
  
  Serial.println("Waiting for player on specific pad");
  
  padsArray[padIndex].isOccupied = false;
  
  if (!espNowSendMsg(padIndex)) {
    return WaitResult::PadMsgDeliveryError;
  }

  // wait until pad is occupied
  unsigned long time_now = millis();
  while (millis() - time_now < defaultDelay) {
    if (padsArray[padIndex].isOccupied) {
      Serial.println("Pad occupied!");
      return WaitResult::PadOccupied;
    }

    if(checkForCancelSignal()) {
      cancelCurrentPadsOperation(padOutput_cancelSayPlayerOnPad);
      return WaitResult::UserAbort;
    }
  }

  Serial.println("Too slow, pad not occupied!");
  return WaitResult::Timeout;
}

std::variant<int, PadsComm::WaitResult> PadsComm::waitForPlayerOnAnyPad() {
  memset(&toSendMsg, 0, sizeof(toSendMsg));
  toSendMsg.function = padOutput_waitForPlayerOnPad;
  toSendMsg.param1[0] = defaultDelay;

  Serial.println("Waiting for player on any pad");

  // send wait signal to all active pads
  for (int i = 0; i < padsCount; i++) {
    padsArray[i].isOccupied = false;
    if (padsArray[i].isActive && !espNowSendMsg(i)) {
      return WaitResult::PadMsgDeliveryError;
    }
  }

  // wait until any pad is occupied
  unsigned long time_now = millis();
  while (millis() - time_now < defaultDelay) {
    for (int i = 0; i < padsCount; i++) {
      if (padsArray[i].isActive && padsArray[i].isOccupied) {
        Serial.println("Any pad occupied!");
        cancelCurrentPadsOperation(padOutput_cancelSayPlayerOnPad);
        return i;
      }
    }

    if(checkForCancelSignal()) {
      cancelCurrentPadsOperation(padOutput_cancelSayPlayerOnPad);
      return WaitResult::UserAbort;
    }
  }

  Serial.println("Too slow, any pad not occupied!");
  return WaitResult::Timeout;
}

PadsComm::WaitResult PadsComm::waitForPlayersOnAllActivePads() {
  memset(&toSendMsg, 0, sizeof(toSendMsg));
  toSendMsg.function = padOutput_waitForPlayerOnPad;
  toSendMsg.param1[0] = defaultDelay;

  Serial.println("Waiting for players on all pads");

  // send wait signal to all active pads
  for (int i = 0; i < padsCount; i++) {
    padsArray[i].isOccupied = false;
    if (padsArray[i].isActive && !espNowSendMsg(i)) {
      return WaitResult::PadMsgDeliveryError;
    }
  }

  // keep track of the order in which the pads were occupied
  uint8_t chronology[padsCount] = { 0 };
  uint8_t padsOccupied = 0;

  // wait until all pads are occupied
  unsigned long time_now = millis();
  while (millis() - time_now < defaultDelay) {
    for (int i = 0; i < padsCount; i++) {
      if (padsArray[i].isActive) {
        if(!padsArray[i].isOccupied) {
          break;
        } else {
          bool alreadyOccupied = false;
          for (int j = 0; j < padsOccupied; j++) {
            if (chronology[j] == i) {
              alreadyOccupied = true;
              break;
            }
          }

          if (!alreadyOccupied) {
            chronology[padsOccupied] = i;
            padsOccupied++;
          }
        }
      }
    }

    if (padsOccupied == padsCount) {
      Serial.println("All active pads occupied!");
      return WaitResult::PadOccupied;
    }

    if (checkForCancelSignal()) {
      cancelCurrentPadsOperation(padOutput_cancelSayPlayerOnPad);
      return WaitResult::UserAbort;
    }
  }

  cancelCurrentPadsOperation(padOutput_cancelSayPlayerOnPad);
  Serial.println("Too slow, not all active pads occupied!");
  return WaitResult::Timeout;
}

bool PadsComm::waitWithCancelCheck(unsigned long ms) {
  unsigned long time_now = millis();
  while (millis() - time_now < ms) {
    if(checkForCancelSignal()) {
      return true;
    }
  }
  return false;
}

bool PadsComm::checkForCancelSignal() {
  yield(); // do WiFi tasks

  if (btComm->hasUnreadBytes()) {
    uint8_t incomingByte = btComm->readByte();

    if (incomingByte == phoneInput_cancel) {
      return true;
    }
  }

  return false;
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

  for (int i = 0; i < padsCount; i++) {
    memcpy(&padsArray[i].macAddr, &padMACAddrs[i], sizeof(padsArray[i].macAddr));
    padsArray[i].isOccupied = false;
    padsArray[i].isActive = true;

    if (esp_now_add_peer(padMACAddrs[i], ESP_NOW_ROLE_COMBO, channel, key, keylen) != 0) {
      padsArray[i].isActive = false;
      Serial.print("Could not add pad ");
    } else {
      Serial.print("Added pad ");
    }

    for (int j = 0; j < 6; j++) {
      Serial.print(padMACAddrs[i][j], HEX);
      if (j < 5)
        Serial.print(":");
      else
        Serial.print("\n");
    }
  }

  Serial.println("Added peers");
}

void PadsComm::cancelCurrentPadsOperation(uint8_t cancelCode) {
  Serial.println("Cancelling operation on all pads");
  toSendMsg.function = cancelCode;
  esp_now_send(broadcastAddress, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
  delay(50);
}

bool PadsComm::espNowSendMsg(uint8_t padIndex) {
  int returnValue = 0;
  if (padIndex < padsCount) {
    if(!padsArray[padIndex].isActive) {
      return false;
    }

    returnValue = esp_now_send(padsArray[padIndex].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
  } else if(padIndex == anyPad) {
    returnValue = esp_now_send(broadcastAddress, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
  } else {
    // Trying to send data to nonexistent pad
    return false;
  }

  delay(5);

  if (returnValue != 0) {
    Serial.println("Error sending message");
    return false;
  }

  return true;
}

void PadsComm::gotMessageFromPad(uint8_t *mac, uint8_t *incomingData) {
  for (int i = 0; i < padsCount; i++) {
    // compare last byte of MAC address, everything else is the same
    if (padsArray[i].macAddr[5] == mac[5]) {
      if (*incomingData == padInput_padOccupied) {
        padsArray[i].isOccupied = true;
      }
      return;
    }
  }
} 

bool PadsComm::isPadOccupied(uint8_t padIndex) const {
  return padIndex < padsCount ? padsArray[padIndex].isOccupied : false;
}