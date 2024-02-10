#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Parser.h"

#define SIGNATURE_LENGTH 4
#define CODE_BUFFER_LENGTH 1000

class Initializer
{
public:
	typedef struct msg_struct {
		unsigned char signature[SIGNATURE_LENGTH + 1]; // +1 for null termination
		int padsCount;
		char code[CODE_BUFFER_LENGTH];
	} msg_struct;

	void onDataRecv(const unsigned int* mac, const char incomingData[CODE_BUFFER_LENGTH], int len);
private:
	msg_struct msg;

	bool validateSignature(unsigned char signature[SIGNATURE_LENGTH]);
};

