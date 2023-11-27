#pragma once

#include <string>
#include <iostream>

#include "ErrorHelper.h"

class Parser
{
private:
	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
public:
	int interpret(size_t len, std::string* code);
};

