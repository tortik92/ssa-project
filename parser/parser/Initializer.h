#pragma once

#include <string>
#include <iostream>

#include "Parser.h"

constexpr int SIGNATURE_LENGTH = 5;
constexpr int CODE_BUFFER_LENGTH = 400;

class Initializer
{
public:
	typedef struct msg_struct {
		char signature[SIGNATURE_LENGTH + 1]; // +1 for null termination
		int padsCount;
		char code[CODE_BUFFER_LENGTH];
	} msg_struct;

	void onDataRecv(const unsigned int* mac, char incomingData[CODE_BUFFER_LENGTH], int len);
private:
	msg_struct msg;

	bool validSignature(char signature[SIGNATURE_LENGTH]);
};

