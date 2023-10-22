#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Parser.h"

class InputStream {
private:
	void getFileLocation(); // calls readFile afterwards
	void readFile(std::string); // calls 
	bool validateFileSignature(unsigned short[3]);
public:
	void read();
};