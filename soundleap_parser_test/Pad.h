#include <cstring>
#pragma once

#include <espnow.h>
#include "Constants.h"
#include "StringHelper.h"

class Pad
{
private:
  uint8_t macAddress[macAddrLen];
  bool active;
	bool occupied;

  typedef struct pad_incoming_msg {
    bool isOccupied;
  } pad_incoming_msg;
  pad_incoming_msg msgIn;

  typedef struct pad_outgoing_msg {
    char command[padCommandLen];
    int musicPlayTime;
    char textToSay[maxAllowedTextToSpeechLen];
  } pad_outgoing_msg;
  pad_outgoing_msg msgOut;
  
public:
	Pad(uint8_t macAddress[macAddrLen]) {
    memcpy(this->macAddress, macAddress, macAddrLen);
    this->active = true;
		this->occupied = false;
	}

	bool isOccupied();
  bool isActive();
  void setActiveStatus(bool isActive);
  void setOccupiedStatus(bool occupied);
	void say(char* text);
	void playMusic(int timeInMs);
	void alarm();
};

