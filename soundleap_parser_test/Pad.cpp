#include "Constants.h"
#include <sys/_stdint.h>
#include "spiffs/spiffs_config.h"
#include "c_types.h"
#include "espnow.h"
#include "Pad.h"

bool Pad::isOccupied() {
  return this->occupied;
}

bool Pad::isActive() {
  return this->active;
}

void Pad::setActiveStatus(bool isActive) {
  this->active = isActive;
}


void Pad::say(char* text) {
  StringHelper::strcpy_s(msgOut.command, padCommandLen, "say");

  int errorCode = StringHelper::strcpy_s(msgOut.textToSay, maxAllowedTextToSpeechLen, text);
  if (errorCode == invalidValue) {
    StringHelper::strcpy_s(msgOut.command, padCommandLen, "Zu sprechender Text ist zu lange");
  }

  esp_now_send(this->macAddress, (uint8_t*)&msgOut, sizeof(msgOut));
}

void Pad::playMusic(int timeInMs) {
  StringHelper::strcpy_s(msgOut.command, padCommandLen, "music");
  msgOut.musicPlayTime = timeInMs;

  esp_now_send(this->macAddress, (uint8_t*)&msgOut, sizeof(msgOut));
}

void Pad::alarm() {
  StringHelper::strcpy_s(msgOut.command, padCommandLen, "alarm");
  esp_now_send(this->macAddress, (uint8_t*)&msgOut, sizeof(msgOut));
}
