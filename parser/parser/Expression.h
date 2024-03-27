#pragma once

#include <iostream>
#include <string>


class Expression
{
private:
	int expression(char*, size_t = 0);
	int term(char*, size_t = 0);
	int factor(char*, size_t = 0);
public:
	// functions
	int parseExpression(char*);
};

