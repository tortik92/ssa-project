#pragma once

#include <string>
#include <iostream>

#include "ErrorHelper.h"

class Parser
{
private:
	std::string* tokenize(std::string line);
public:
	int interpret(size_t len, std::string* code);
};

