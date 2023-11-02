#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Parser.h"

class InputStream {
private:
	std::string getFileLocation(); // gets called by readFile
	bool validateFileSignature(unsigned short[3]);
public:
	void readFile();
};