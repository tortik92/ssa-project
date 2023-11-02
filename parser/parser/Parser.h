#pragma once

#include <string>
#include <iostream>

#include "ErrorHelper.h"

class Parser
{
private:
public:
	int interpret(size_t len, std::string* code);
};

