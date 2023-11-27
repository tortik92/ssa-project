#pragma once

#include <string>
#include <iostream>

#include <stdlib.h>

#include "Pad.h"

class Parser
{
private:
	int nrOfPads;
	Pad* pads;

	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
	int parseNumber(std::string number, bool requiredPositive);
	int parseNumber(std::string number, bool requiredPositive, int maxValue); // calls parseNumber with 2 params (the one above)
public:
	Parser(int nrOfPads) {
		this->nrOfPads = nrOfPads;
		this->pads = new Pad[nrOfPads];
	}
	int interpret(size_t len, std::string* code);
};

