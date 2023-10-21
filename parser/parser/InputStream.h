#pragma once

#include <iostream>
#include <fstream>
#include <string>

class InputStream {
private:
public:
	std::string getFileLocation();
	std::string* readFile(std::string);
	bool validateFileSignature(unsigned short[3]);
};