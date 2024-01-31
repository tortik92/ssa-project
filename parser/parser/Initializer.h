#pragma once

#include <string>
#include <iostream>

#include "Parser.h"

#define FILE_SIGNATURE_LENGTH 4
#define CODE_BUFFER_LENGTH 1000

class Initializer
{
public:
	typedef struct msg_struct {
		unsigned char signature[FILE_SIGNATURE_LENGTH];
		int padsCount;
		char code[CODE_BUFFER_LENGTH];
	} msg_struct;

	void onDataRecv(const unsigned int* mac, const unsigned char* incomingData, int len);
private:
	msg_struct msg;

	bool validateSignature(unsigned char signature[3]);
};

