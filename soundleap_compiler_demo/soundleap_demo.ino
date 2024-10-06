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

const uint8_t padOutput_isPlayerOnPad = 0x10;
const uint8_t padOutput_playSound8Val = 0x20;
const uint8_t padOutput_waitForPlayerOnPad = 0x30;

// other constants
const uint8_t maxAllowedPads = 4;
const uint8_t reaktionPlayerCount = 2;
const uint8_t paramLen = 8;
const uint8_t chordAMajorLen = 4;

const int chordAMajor[paramLen] = { 440, 550, 660, 880, 0, 0, 0, 0 };  // tone values in Hz, 4 times zero to make array length of 8
const int defaultBeat[paramLen] = {};

// ------------------VARIABLES---------------------
uint8_t myMACAddr[] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 };

uint8_t padMACAddrs[maxAllowedPads][6] = {
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x04 }
};

uint8_t broadcastAddress[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

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
  uint8_t MACAddress[6];
  bool isOccupied;
  bool isActive;
} pad;

pad padsArray[maxAllowedPads];
uint8_t eventOrder[maxAllowedPads] = { 0 };

uint8_t activePadCount = 0;
bool isTrackingEvents = false;
bool isAnyPadOccupied = false;

// ----------------MAIN FUNCTIONS-----------------
void setup() {
  // init connections
  Serial.begin(9600);
  BTserial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_macaddr(STATION_IF, &myMACAddr[0]);

  if (esp_now_init() != ERR_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // add peers
  uint8_t channel = 1;
  uint8_t *key = NULL;
  uint8_t keylen = 0;

  for (int i = 0; i < maxAllowedPads; i++) {
    // channel key keylen
    esp_now_add_peer(padMACAddrs[i], ESP_NOW_ROLE_COMBO, channel, key, keylen);
    memcpy(&padsArray[i].MACAddress, &padMACAddrs[i], sizeof(padsArray[i].MACAddress));
    padsArray[i].isOccupied = false;
    padsArray[i].isActive = false;
  }

  // random seed
  randomSeed(analogRead(0));
}

void loop() {
  char gameSelection = ' ';

  if (BTserial.available()) {
    gameSelection = BTserial.read();
    switch (gameSelection) {
      case phoneInput_makeSound_pad1:
        playSingleSoundOnPad(padsArray[0].MACAddress, chordAMajor[0], 1000);
        break;
      case phoneInput_makeSound_pad2:
        playSingleSoundOnPad(padsArray[1].MACAddress, chordAMajor[1], 1000);
        break;
      case phoneInput_makeSound_pad3:
        playSingleSoundOnPad(padsArray[2].MACAddress, chordAMajor[2], 1000);
        break;
      case phoneInput_makeSound_pad4:
        playSingleSoundOnPad(padsArray[3].MACAddress, chordAMajor[3], 1000);
        break;
      case phoneInput_gameSelection_Memory:
        {
          Serial.println("Memory selected");

          const uint8_t padSeqLen = 128;
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
            playSingleSoundOnPad(padsArray[selectedPad].MACAddress, chordAMajor[selectedPad], 1000);

            // check if player jumps on correct pad
            waitForPlayerOnAnyPad();
            if (padsArray[selectedPad].isOccupied == false) {
              playWrongActionJingle();
              break;
            } else if (correctSelectionsCount >= padSeqLen) {
              playWinnerJingle();
            } else {
              playCorrectActionJingle();
              padSeq[correctSelectionsCount] = selectedPad;
              correctSelectionsCount++;
            }
          }

          Serial.println("Memory end");
          break;
        }
      case phoneInput_gameSelection_Reaktion:
        {
          Serial.println("Reaktion selected");
          /*
          1. Zu Beginn des Spiels wird ein richtiger Ton abgespielt. 
          2. Danach werden Töne in zufälliger Frequenz abgespielt. 
          3. Ziel des Spiels ist es, beim richtigen Ton so schnell wie möglich auf die Matte zu steigen, 
             wobei der langsamste Spieler ausscheidet. 
          4. Es werden so viele Runden gespielt, bis ein/e Gewinner/in feststeht.
          */

          activePadCount = 2;
          int shuffledToneOrder[chordAMajorLen] = { 0 };
          memcpy(&shuffledToneOrder, &chordAMajor, sizeof(shuffledToneOrder));
          int correctToneOrder[chordAMajorLen] = { 0 };
          int soundLenArray[paramLen] = { 1000, 1000, 1000, 1000, 0, 0, 0, 0 };

          while (activePadCount > 1) {
            // play correct sound
            shuffle(shuffledToneOrder, chordAMajorLen);
            memcpy(&correctToneOrder, &shuffledToneOrder, sizeof(correctToneOrder));
            play8Sounds(correctToneOrder, soundLenArray);
            shuffle(shuffledToneOrder, chordAMajorLen);

            while (memcmp(shuffledToneOrder, correctToneOrder, sizeof(chordAMajor)) != 0) {
              shuffle(shuffledToneOrder, chordAMajorLen);
              play8Sounds(shuffledToneOrder, soundLenArray);
            }

            waitForPlayersOnAllActivePads();

            // play correct for all except winner and loser
            for (int i = 1; i < activePadCount - 1; i++) {
              playCorrectActionJingleOnPad(eventOrder[i]);
            }

            // play winner and loser hingle
            uint8_t winner = eventOrder[0];
            uint8_t loser = eventOrder[activePadCount - 1];

            playWinnerJingleOnPad(winner);
            playWrongActionJingleOnPad(loser);

            // remove loser from round
            padsArray[loser].isActive = false;
            activePadCount--;
          }

          Serial.println("Reaktion end");
          break;
        }
      default:
        Serial.println("Game shortcut not defined");
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

void prepareSend() {
  toSendMsg.function = 0;

  for (int i = 0; i < paramLen; i++) {
    memset(&toSendMsg.param1[i], 0, sizeof(toSendMsg.param1));
    memset(&toSendMsg.param2[i], 0, sizeof(toSendMsg.param2));
    memset(&toSendMsg.param3[i], 0, sizeof(toSendMsg.param3));
    memset(&toSendMsg.param4[i], 0, sizeof(toSendMsg.param4));
  }
}

bool waitWithAbortCheck(unsigned long millisecs) {
  unsigned long time_now = millis();

  while (millis() - time_now < millisecs) {  // equivalent to millis() < time_now + millisecs but with overflow security
    if (BTserial.available()) {
      uint8_t incomingByte = BTserial.read();

      if (incomingByte == phoneInput_cancelGame) return true;
    }
  }

  return false;
}

// ----------ABSTRACTED PAD CALL FUNCTIONS-----------
void waitForPlayerOnPad(unsigned char padIndex) {
  padsArray[padIndex].isOccupied = false;
  prepareSend();
}

void waitForPlayerOnAnyPad() {
  for (int i = 0; i < activePadCount; i++) {
    padsArray[i].isOccupied = false;
  }
}

void waitForPlayersOnAllActivePads() {
  isTrackingEvents = true;
}

void playSingleSoundOnPad(uint8_t macAddr[6], int soundVal, int soundLenMs) {
  prepareSend();

  toSendMsg.function = padOutput_playSound8Val;
  toSendMsg.param1[0] = soundVal;
  toSendMsg.param2[0] = soundLenMs;

  esp_now_send(macAddr, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
}

void playSingleSound(int soundVal, int soundLenMs) {
  playSingleSoundOnPad(broadcastAddress, soundVal, soundLenMs);
}

void play8Sounds(int soundVal[paramLen], int soundLenMs[paramLen]) {
}

void playCorrectActionJingle() {
}

void playCorrectActionJingleOnPad(uint8_t padIndex) {
}

void playWrongActionJingle() {
}

void playWrongActionJingleOnPad(uint8_t padIndex) {
}

void playWinnerJingle() {
}

void playWinnerJingleOnPad(uint8_t padIndex) {
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
  // add event order
  if (isTrackingEvents) {
    for (int i = 0; i < maxAllowedPads; i++) {
      if (memcmp(padsArray[i].MACAddress, mac, 6) == 0) {
        int j = findFirstEmptySlot(eventOrder, maxAllowedPads);
        if (j != -1) {
          eventOrder[j] = i;
        } else {
          Serial.println("eventOrder array overflow detected");
        }
      }
    }
  }

  memcpy(&toRecvMsg, incomingData, sizeof(toRecvMsg));
}