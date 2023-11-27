#pragma once

#include <string>
#include <iostream>

#include <stdlib.h>

#include "ErrorHelper.h"

class Parser
{
private:
	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
	int parseNumber(std::string number, bool requiredPositive);
	int parseNumber(std::string number, bool requiredPositive, int maxValue);
public:
	int interpret(size_t len, std::string* code);
};

