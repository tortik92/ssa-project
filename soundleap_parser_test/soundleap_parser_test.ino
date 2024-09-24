#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Parser.h"
#include "Constants.h"

uint8_t myMACAddr[] = { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x00 };

uint8_t padMACAddresses[maxSupportedPads][macAddrLen] = {
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x01 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x02 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x03 },
  { 0x92, 0x53, 0x54, 0x4C, 0x50, 0x04 }
};

typedef struct msg_struct {
  char signature[signatureLen + 1];  // +1 for null termination
  int padsCount;
  char code[codeBufferLen];
} msg_struct;
msg_struct msg;

void setup() {
  // initialize serial connections
  Serial.begin(9600);

  // reset Wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_macaddr(STATION_IF, myMACAddr);
  Serial.println("My MAC Addr: " + WiFi.macAddress());

  // initialize ESP_NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_add_peer(padMACAddresses[0], ESP_NOW_ROLE_COMBO, 1, NULL, 0);

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(onDataRecv);
}

void loop() {
}

void onDataRecv(uint8_t* mac, unsigned char incomingData[codeBufferLen], size_t len) {
  Serial.println("Incoming message from " + mac);
  memcpy(&msg, incomingData, sizeof(msg));
  if (validSignature(msg.signature)) {
    Parser parser(msg.padsCount, padMACAddresses);

    // extract code
    char code[linesCount][codeLineLen];

    char* token = strtok(msg.code, "\n");
    for (int i = 0; i < linesCount; i++) {
      if (token != NULL) {
        StringHelper::strcpy_s(code[i], codeBufferLen, token);

        token = strtok(NULL, "\n");
      } else {
        StringHelper::strcpy_s(code[i], codeBufferLen, "\0");
      }
    }

    char errmsg[40];
    parser.interpret(code, errmsg);

    Serial.println(errmsg);
  } else {
    Serial.print("Invalid signature");
  }
}

bool validSignature(char fileSignature[signatureLen]) {
  if (fileSignature[0] == 'S' && fileSignature[1] == 'D' && fileSignature[2] == 'L' && fileSignature[3] == 'P') {
    return true;
  }
  return false;
}

// Callback for when data is sent
void OnDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  Serial.print("Send Status: ");
  if (sendStatus == 0) {
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
}
