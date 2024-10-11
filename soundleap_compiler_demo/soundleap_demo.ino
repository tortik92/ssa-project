// -------------------INCLUDE-----------------------
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// ------------------CONSTANTS----------------------
// BLE inputs
const uint8_t phoneInput_makeSound_pad1 = 0x01;
const uint8_t phoneInput_makeSound_pad2 = 0x02;
const uint8_t phoneInput_makeSound_pad3 = 0x03;
const uint8_t phoneInput_makeSound_pad4 = 0x04;
const uint8_t phoneInput_gameSelection_Memory = 0x10;
const uint8_t phoneInput_gameSelection_Reaktion = 0x11;
const uint8_t phoneInput_cancelGame = 0xFF;

const uint8_t padOutput_playSound8Val = 0x20;
const uint8_t padOutput_waitForPlayerOnPad = 0x30;
const uint8_t padOutput_cancelOperation = 0xFF;

const uint8_t padInput_padOccupied = 0x11;

// other constants
const uint8_t maxAllowedPads = 3;
const uint8_t reaktionPlayerCount = 2;
const uint8_t paramLen = 8;
const uint8_t chordAMajorLen = 4;
const unsigned long defaultDelay = 10000;  // 10 seconds

// music
const int gameActionTones[paramLen] = { 440, 550, 660, 880, 0, 0, 0, 0 };
const int gameActionDurations[paramLen] = { 100, 100, 100, 200, 0, 0, 0, 0 };

const int gameConclusionTones[paramLen] = { 440, 550, 660, 550, 660, 880, 0, 0 };
const int gameConclusionDurations[paramLen] = { 125, 125, 125, 125, 125, 125, 0, 0 };

const int chordAMajor[paramLen] = { 440, 550, 660, 880, 0, 0, 0, 0 };  // tone values in Hz, 4 times zero to make array length of 8
const int defaultBeat[paramLen] = { 200, 200, 400, 400, 200, 200, 0, 0 };

// --------FUNCTION PROTOTYPE DEFINITONS----------
// Due to default value assignment, these functions have to be defined before the implementation
uint8_t playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex = UINT8_MAX);
uint8_t play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex = UINT8_MAX);
uint8_t playCorrectActionJingle(uint8_t padIndex = UINT8_MAX);
uint8_t playWrongActionJingle(uint8_t padIndex = UINT8_MAX);
uint8_t playWinnerJingle(uint8_t padIndex = UINT8_MAX);
uint8_t playLoserJingle(uint8_t padIndex = UINT8_MAX);

// ------------------VARIABLES---------------------
// MAC Addresses
uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t myMACAddr[] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 };
uint8_t padMACAddrs[maxAllowedPads][6] = {
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 }/*,
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x04 }*/
};

SoftwareSerial BTserial(D6, D7);  // RX - TX

typedef struct struct_recv_msg {
  uint8_t returnValue;
} struct_recv_msg;

typedef struct struct_send_msg {
  uint8_t function;
  int param1[paramLen];
  int param2[paramLen];
  int param3[paramLen];
  int param4[paramLen];
} struct_send_msg;

struct_recv_msg toRecvMsg;
struct_send_msg toSendMsg;

typedef struct pad {
  uint8_t macAddr[6];
  bool isOccupied;
  bool isActive;
} pad;

pad padsArray[maxAllowedPads];

uint8_t activePadCount = 0;

// event tracking & flags
uint8_t eventOrder[maxAllowedPads] = { 0 };
bool eventTrackingFlag = false;
uint8_t waitingForSpecificPadOccupied = UINT8_MAX;
bool waitingForAnyPadOccupied = false;
bool waitingForAllPadsOccupied = false;
bool anyPadOccupied = false;
bool allPadsOccupied = false;

bool cancelFlag = false;

// ----------------MAIN FUNCTIONS-----------------
void setup() {
  // init connections
  Serial.begin(115200);
  BTserial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_macaddr(STATION_IF, &myMACAddr[0]);

  if (esp_now_init() != ERR_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Successfully initialized ESP-NOW");
  }

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

    if(esp_now_add_peer(padMACAddrs[i], ESP_NOW_ROLE_COMBO, channel, key, keylen) != 0) {
      Serial.println("Failed to add peer");
      padsArray[i].isActive = false;
    } else {
      printWithMac("Added peer", padMACAddrs[i]);
    }
  }

  Serial.println("Added peers");

  // random seed
  randomSeed(analogRead(0));
}

void loop() {
  char gameSelection = ' ';

  if (BTserial.available()) {
    gameSelection = BTserial.read();

    switch (gameSelection) {
      case phoneInput_makeSound_pad1:
        playSingleSound(chordAMajor[0], 1000, 0);
        break;
      case phoneInput_makeSound_pad2:
        playSingleSound(chordAMajor[1], 1000, 1);
        break;
      case phoneInput_makeSound_pad3:
        playSingleSound(chordAMajor[2], 1000, 2);
        break;
      case phoneInput_makeSound_pad4:
        playSingleSound(chordAMajor[3], 1000, 3);
        break;
      case phoneInput_gameSelection_Memory:
        {
          Serial.println("Memory selected");

          const uint8_t padSeqLen = 32;
          uint8_t correctSelectionsCount = 0;
          uint8_t padSeq[padSeqLen] = { 0 };
          uint8_t padNotSelectable = UINT8_MAX;
          uint8_t selectedPad = UINT8_MAX;


          while (true) {
            // select random pad and deactivate for next round
            selectedPad = random(maxAllowedPads);
            while (selectedPad == padNotSelectable) {
              selectedPad = random(maxAllowedPads);
            }
            padNotSelectable = selectedPad;

            // play tone on pad
            playSingleSound(chordAMajor[selectedPad], 1000, selectedPad);

            // check if player jumps on correct pad
            if (waitForPlayerOnAnyPad() == phoneInput_cancelGame) return;

            if (padsArray[selectedPad].isOccupied == false) {
              if (playWrongActionJingle() == phoneInput_cancelGame) return;
              else break;
            } else {
              padSeq[correctSelectionsCount] = selectedPad;
              correctSelectionsCount++;

              if (correctSelectionsCount >= padSeqLen && playWinnerJingle() == phoneInput_cancelGame) return;
              else if (playCorrectActionJingle() == phoneInput_cancelGame) return;
            }
          }

          Serial.println("Memory end");
          break;
        }
      case phoneInput_gameSelection_Reaktion:
        {
          Serial.println("Reaktion selected");

          activePadCount = 2;
          int shuffledToneOrder[chordAMajorLen] = { 0 };
          memcpy(&shuffledToneOrder, &chordAMajor, sizeof(shuffledToneOrder));
          int correctToneOrder[chordAMajorLen] = { 0 };
          int soundLenArray[paramLen] = { 1000, 1000, 1000, 1000, 0, 0, 0, 0 };

          while (activePadCount > 1) {
            // play correct sound
            shuffle(shuffledToneOrder, chordAMajorLen);
            memcpy(&correctToneOrder, &shuffledToneOrder, sizeof(correctToneOrder));
            if (play8Sounds(correctToneOrder, soundLenArray) == phoneInput_cancelGame) return;
            shuffle(shuffledToneOrder, chordAMajorLen);

            while (memcmp(shuffledToneOrder, correctToneOrder, chordAMajorLen * sizeof(int)) != 0) {
              shuffle(shuffledToneOrder, chordAMajorLen);
              if (play8Sounds(shuffledToneOrder, soundLenArray) == phoneInput_cancelGame) return;
            }


            uint8_t ret = waitForPlayersOnAllActivePads();
            if (ret == phoneInput_cancelGame || ret == 0) return;  // end game if somebody didn't get on the pads in time

            // play correct for all except winner and loser
            for (int i = 1; i < activePadCount - 1; i++) {
              uint8_t padIndex = eventOrder[i];
              if (playCorrectActionJingle(padIndex) == phoneInput_cancelGame) return;
            }

            // play winner and loser hingle
            uint8_t winnerPad = eventOrder[0];
            uint8_t loserPad = eventOrder[activePadCount - 1];

            if (playWinnerJingle(winnerPad) == phoneInput_cancelGame) return;
            if (playWrongActionJingle(loserPad) == phoneInput_cancelGame) return;

            // remove loser from round
            padsArray[loserPad].isActive = false;
            activePadCount--;
          }

          Serial.println("Reaktion end");
          break;
        }
      default:
        break;
    }
  }
}

// ----------------HELPER FUNCTIONS------------------
void shuffle(int *array, size_t n) {
  if (n > 1) {
    for (size_t i = 0; i < n; i++) {
      size_t j = random(i, n - 1);

      int tmp = array[j];
      array[j] = array[i];
      array[i] = tmp;
    }
  }
}

int findFirstEmptySlot(uint8_t *arr, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (arr[i] == 0) return i;
  }
  return -1;
}

void prepareWait() {
  waitingForAnyPadOccupied = false;
  anyPadOccupied = false;
  waitingForAllPadsOccupied = false;
  allPadsOccupied = false;
  waitingForSpecificPadOccupied = UINT8_MAX;
  memset(eventOrder, 0, sizeof(maxAllowedPads));
}

void prepareSend() {
  // set toSendMsg struct to 0
  toSendMsg.function = 0;

  memset(toSendMsg.param1, 0, sizeof(toSendMsg.param1));
  memset(toSendMsg.param2, 0, sizeof(toSendMsg.param2));
  memset(toSendMsg.param3, 0, sizeof(toSendMsg.param3));
  memset(toSendMsg.param4, 0, sizeof(toSendMsg.param4));

  // set toRecvMsg struct to 0
  toRecvMsg.returnValue = 0xFF;
}

uint8_t waitWithEventChecks(unsigned long ms) {
  unsigned long time_now = millis();

  while (millis() - time_now < ms) {
    if (BTserial.available()) {
      uint8_t incomingByte = BTserial.read();

      if (incomingByte == phoneInput_cancelGame) {
        cancelOperation();
        return phoneInput_cancelGame;
      }
    }

    if (waitingForAnyPadOccupied) {
      if (anyPadOccupied) {
        cancelOperation();
        return padInput_padOccupied;
      }
    } else if (waitingForAllPadsOccupied) {
      if (allPadsOccupied) {
        cancelOperation();
        return padInput_padOccupied;
      }
    } else if (waitingForSpecificPadOccupied != UINT8_MAX) {
      if (padsArray[waitingForSpecificPadOccupied].isOccupied) {
        cancelOperation();
        return padInput_padOccupied;
      }
    }
  }

  Serial.println("Timeout");
  return 0;
}

void cancelOperation() {
  waitingForAnyPadOccupied = false;
  waitingForAllPadsOccupied = false;
  waitingForSpecificPadOccupied = UINT8_MAX;

  prepareSend();
  toSendMsg.function = padOutput_cancelOperation;
  for (uint8_t i = 0; i < maxAllowedPads; i++) {
    esp_now_send(padsArray[i].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
  }
}

void printWithMac(const char* msg, uint8_t* mac) {
  Serial.print(msg);
  Serial.write(' ');
  for(int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
    else Serial.print("\n");
  }
}

// ----------ABSTRACTED PAD CALL FUNCTIONS-----------
uint8_t waitForPlayerOnPad(int padIndex) {
  padsArray[padIndex].isOccupied = false;
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  waitingForSpecificPadOccupied = padIndex;

  Serial.println();
  printWithMac("Waiting for player on pad", padsArray[padIndex].macAddr);

  esp_now_send(padsArray[padIndex].macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  return waitWithEventChecks(defaultDelay);
}

uint8_t waitForPlayerOnAnyPad() {
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  waitingForAnyPadOccupied = true;

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

uint8_t waitForPlayersOnAllActivePads() {
  prepareSend();
  toSendMsg.function = padOutput_waitForPlayerOnPad;

  prepareWait();
  waitingForAllPadsOccupied = true;

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

uint8_t playSingleSound(const int soundVal, const int soundLenMs, uint8_t padIndex /*= UINT8_MAX*/) {
  prepareSend();

  toSendMsg.function = padOutput_playSound8Val;
  toSendMsg.param1[0] = soundVal;
  toSendMsg.param2[0] = soundLenMs;

  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;

  printWithMac("Playing single sound on pad", macAddr);

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  return waitWithEventChecks(soundLenMs);
}

uint8_t play8Sounds(const int soundVal[paramLen], const int soundLenMs[paramLen], uint8_t padIndex /*= UINT8_MAX*/) {
  uint8_t *macAddr = padIndex < maxAllowedPads ? padsArray[padIndex].macAddr : broadcastAddress;

  toSendMsg.function = padOutput_playSound8Val;
  memcpy(toSendMsg.param1, soundVal, paramLen);
  memcpy(toSendMsg.param2, soundLenMs, paramLen);

  printWithMac("Playing 8 sounds on pad", macAddr);

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));

  unsigned long delay = 0;
  for (int i = 0; i < paramLen; i++) {
    delay += soundLenMs[i];
  }

  return waitWithEventChecks(delay);
}

uint8_t playCorrectActionJingle(uint8_t padIndex /*= UINT8_MAX*/) {
  return play8Sounds(gameActionTones, gameActionDurations, padIndex);
}

uint8_t playWrongActionJingle(uint8_t padIndex /*= UINT8_MAX*/) {
  int reverseTones[paramLen] = { 0 };
  for (size_t i = 0; i < paramLen; i++) {
    reverseTones[paramLen - 1 - i] = gameActionTones[i];
  }
  return play8Sounds(reverseTones, gameActionDurations, padIndex);
}

uint8_t playWinnerJingle(uint8_t padIndex /*= UINT8_MAX*/) {
  return play8Sounds(gameConclusionTones, gameConclusionDurations, padIndex);
}

uint8_t playLoserJingle(uint8_t padIndex /*= UINT8_MAX*/) {
  int reverseTones[paramLen] = { 0 };
  for (size_t i = 0; i < paramLen; i++) {
    reverseTones[paramLen - 1 - i] = gameConclusionTones[i];
  }
  return play8Sounds(reverseTones, gameConclusionDurations, padIndex);
}

// ----------------CALLBACK FUNCTIONS-------------------
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("\nMessage sent to: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac_addr[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println(sendStatus == ERR_OK ? " Success" : " Failure");
}

void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (waitingForAnyPadOccupied) {
    anyPadOccupied = true;
  } else if (waitingForSpecificPadOccupied < maxAllowedPads) {
    if (memcmp(padsArray[waitingForSpecificPadOccupied].macAddr, mac, 6) == 0) {
    }
  } else if (waitingForAllPadsOccupied) {
    for (int padIndex = 0; padIndex < maxAllowedPads; padIndex++) {
      if (memcmp(padsArray[padIndex].macAddr, mac, 6) == 0) {
        int emptySlot = findFirstEmptySlot(eventOrder, maxAllowedPads);

        if (emptySlot != -1) {
          allPadsOccupied = emptySlot == maxAllowedPads - 1;
          eventOrder[emptySlot] = padIndex;
        } else {
          Serial.println("eventOrder array overflow detected");
        }
      }
    }
  }

  memcpy(&toRecvMsg, incomingData, sizeof(toRecvMsg));
}