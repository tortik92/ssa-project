#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "HX711.h"

// instruction values
const uint8_t play8Sounds = 0x20;
const uint8_t play16Sounds = 0x21;
const uint8_t sayWhenPlayerOnPad = 0x30;
const uint8_t cancelPlaySound = 0xFE;
const uint8_t cancelSayPlayerOnPad = 0xFF;

// return values
const uint8_t ret_timeout = 0x00;
const uint8_t ret_padOccupied = 0x01;
const uint8_t ret_cancel = 0xFF;

// MAC-Address
uint8_t myMACAddress[] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 };

// Main-Module-Address
uint8_t mainMACAddress[] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 };

HX711 scale;

// Pins7
#define LOADCELL_DOUT_PIN D5
#define LOADCELL_SDK_PIN D6
#define BUZZER_PIN D2

#define MESSAGE_QUEUE_SIZE 10

typedef struct SendMsg {
  uint8_t returnValue;
} SendMsg;

typedef struct RecvMsg {
  uint8_t function = 0x00;
  int param1[8] = { 0 };
  int param2[8] = { 0 };
  int param3[8] = { 0 };
  int param4[8] = { 0 };
} RecvMsg;

SendMsg toSendMsg;

RecvMsg messageQueue[MESSAGE_QUEUE_SIZE];
volatile int queueHead = 0;
volatile int queueTail = 0;

bool pushMessage(const RecvMsg &msg) {
  int nextTail = (queueTail + 1) % MESSAGE_QUEUE_SIZE;
  if (nextTail == queueHead) {
    Serial.println("Message queue full!");
    return false;
  }
  messageQueue[queueTail] = msg;
  queueTail = nextTail;

  Serial.print("Enqueued: [");
  for (int i = queueHead; i != queueTail; i = (i + 1) % MESSAGE_QUEUE_SIZE) {
    if (messageQueue[i].function != 0x00) {
      Serial.print(messageQueue[i].function, HEX);
      if ((i + 1) % MESSAGE_QUEUE_SIZE != queueTail) {
        Serial.print(", ");
      }
    }
  }
  Serial.println("]");

  return true;
}

bool popMessage(RecvMsg &msg) {
  if (queueHead == queueTail) {
    return false;
  }
  msg = messageQueue[queueHead];
  queueHead = (queueHead + 1) % MESSAGE_QUEUE_SIZE;

  Serial.print("Dequeued: [");
  for (int i = queueHead; i != queueTail; i = (i + 1) % MESSAGE_QUEUE_SIZE) {
    if (messageQueue[i].function != 0x00) {
      Serial.print(messageQueue[i].function, HEX);
      if ((i + 1) % MESSAGE_QUEUE_SIZE != queueTail) {
        Serial.print(", ");
      }
    }
  }
  Serial.println("]");

  return true;
}

uint8_t peekNextFunction() {
  if (queueHead == queueTail) {
    return 0;
  } else {
    return messageQueue[queueHead].function;
  }
}

void printMac(uint8_t *mac) {
  for (size_t i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
}

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("[OUTGOING] ");
  printMac(mac_addr);
  if (sendStatus == 0) {
    Serial.println(" Success");
  } else {
    Serial.println(" Failure");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  RecvMsg tempMsg;
  memcpy(&tempMsg, incomingData, sizeof(tempMsg));

  Serial.print("[INCOMING] ");
  printMac(mac);
  Serial.print(" Command: ");

  switch (tempMsg.function) {
    case play8Sounds:
      Serial.println("Play 8 sounds");
      break;
    case play16Sounds:
      Serial.println("Play 16 sounds");
      break;
    case sayWhenPlayerOnPad:
      Serial.println("Say when player on pad");
      break;
    case cancelPlaySound:
      Serial.println("Cancel play sound");
      break;
    case cancelSayPlayerOnPad:
      Serial.println("Cancel say player on pad");
      break;
    default:
      break;
  }

  pushMessage(tempMsg);
}

void sendResponse(uint8_t returnValue) {
  Serial.print("[DEBUG]: Sending response 0x");
  Serial.println(returnValue, HEX);
  toSendMsg.returnValue = returnValue;
  esp_now_send(mainMACAddress, (uint8_t *)&toSendMsg, sizeof(toSendMsg));
}

void handleCancelRequest() {
  Serial.println("[DEBUG]: Sending cancel response");
  RecvMsg tmpMsg;
  popMessage(tmpMsg);
  sendResponse(tmpMsg.function);
}

void play8Sound(int soundVal[8], int soundLen[8]) {
  Serial.print("Play 8 sounds: [");
  for (int i = 0; i < 8; i++) {
    if(soundLen[i] == 0) {
      break;
    }
    Serial.print(soundVal[i]);
    if (i < 7 && soundLen[i + 1] != 0) Serial.print(", ");
  }
  Serial.print("] | Lengths: [");
  for (int i = 0; i < 8; i++) {
    if(soundLen[i] == 0) {
      break;
    }
    Serial.print(soundLen[i]);
    if (i < 7 && soundLen[i + 1] != 0) Serial.print(", ");
  }
  Serial.println("]");

  for (int i = 0; i < 8; i++) {
    if (peekNextFunction() == cancelPlaySound) {
      return;
    }

    if (soundLen[i] != 0) {
      tone(BUZZER_PIN, soundVal[i], soundLen[i]);
      delay(soundLen[i] * 1.2);
      noTone(BUZZER_PIN);
    }
  }
}

// Process play8Sounds command
void play8SoundMessage(RecvMsg msg) {
  play8Sound(msg.param1, msg.param2);
  if (peekNextFunction() == cancelPlaySound) {
    Serial.println("Cancelling play8SoundMessage");
    handleCancelRequest();
  } else {
    sendResponse(ret_timeout);
  }
}

// Process play16Sounds command
void play16SoundMessage(RecvMsg msg) {
  play8Sound(msg.param1, msg.param2);
  play8Sound(msg.param3, msg.param4);
  if (peekNextFunction() == cancelPlaySound) {
    handleCancelRequest();
  } else {
    sendResponse(ret_timeout);
  }
}

// Process sayWhenPlayerOnPad command and allow cancellation
void sayPlayerOnPadMessage(RecvMsg msg) {
  unsigned long start = millis();
  unsigned long timeoutMs = msg.param1[0];
  while (millis() - start < timeoutMs) {
    yield();  // do WiFi tasks

    if (peekNextFunction() == cancelSayPlayerOnPad) {
      Serial.println("cancelling sayPlayerOnPadMessage");
      handleCancelRequest();
      return;
    } else if (scale.get_units() <= -7000) {
      Serial.println("Player is on pad");
      sendResponse(ret_padOccupied);
      return;
    }
  }

  Serial.print("Too slow, no player on pad after ");
  Serial.print(timeoutMs);
  Serial.println(" ms");
  sendResponse(ret_timeout);
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Change MAC
  wifi_set_macaddr(STATION_IF, &myMACAddress[0]);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // Prepare pins
  pinMode(BUZZER_PIN, OUTPUT);

  // Configure ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(mainMACAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  esp_now_register_recv_cb(OnDataRecv);

  // Configure weight sensors
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SDK_PIN);
  scale.tare();

  RecvMsg startingMelody;

  startingMelody.function = play8Sounds;
  startingMelody.param1[0] = 880;
  startingMelody.param1[1] = 1760;
  startingMelody.param1[2] = 3520;
  startingMelody.param2[0] = 100;
  startingMelody.param2[1] = 100;
  startingMelody.param2[2] = 100;

  pushMessage(startingMelody);
}

void loop() {
  RecvMsg currentMsg;
  if (popMessage(currentMsg)) {
    switch (currentMsg.function) {
      case play8Sounds:
        play8SoundMessage(currentMsg);
        break;
      case play16Sounds:
        play16SoundMessage(currentMsg);
        break;
      case sayWhenPlayerOnPad:
        sayPlayerOnPadMessage(currentMsg);
        break;
      default:
        break;
    }
  }
}
