#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Parser.h"
#include "ErrorHelper.h"

class InputStream {
private:
	const char* getFileLocation(); // gets called by readFile
	bool validateFileSignature(unsigned short[3]);
public:
	void readFile();
};