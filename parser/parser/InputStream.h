#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Parser.h"

class InputStream {
private:
	unsigned short FILE_SIGNATURE_LENGTH = 6;
	unsigned short FIRST_LINE_LENGTH = 7;

	const char* getFileLocation(); // gets called by readFile
	bool validateFileSignature(unsigned short[3]);
public:
	void readFile();
};