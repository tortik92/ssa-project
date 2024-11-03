#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <SoftwareSerial.h>

class BLEComm {
public:
    BLEComm(const BLEComm &copy) = delete;
    
    static BLEComm* getInstance() {
        if(instance == nullptr) {
            instance = new BLEComm();
        }
        return instance;
    }

    bool hasUnreadBytes();
    uint8_t readByte();
private:
    static BLEComm* instance;

    BLEComm() {
        BTserial.begin(9600);
    }

    SoftwareSerial BTserial = SoftwareSerial(D6, D7);  // RX - TX
};