#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "Pad.h"

class Parser
{
private:
	// variables & objects
	unsigned short padsCount;
	
	Pad* pads;

	// parsing
	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
	int parseNumber(std::string number, bool requiredPositive);
	int parseNumber(std::string number, bool requiredPositive, int maxValue); // calls parseNumber with 2 params (above)
	bool parseIf(std::string condition);
	std::string* parseOperator(std::string line, std::string operatorToken);

	// other
	void deactivateActivePad(short index);
public:
	Parser(unsigned short padsCount) {
		std::srand((unsigned int)std::time(nullptr));

		this->padsCount = padsCount;
		this->pads = new Pad[padsCount];
	}
	~Parser() {
		delete[] pads;
	}
	int interpret(size_t len, std::string* code);
};

