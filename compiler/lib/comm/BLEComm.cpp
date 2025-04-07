#include "BLEComm.h"

BLEComm* BLEComm::instance = nullptr;

bool BLEComm::hasUnreadBytes() {
  return BTserial.available();
}

uint8_t BLEComm::readByte() {
  return BTserial.read();
}

String BLEComm::readCode() {
  return BTserial.readStringUntil('\n');
}