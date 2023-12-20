#pragma once

#include <iostream>
#include <string>


class Expression
{
private:
	// variables & objects
	unsigned short padsCount;

	float expression(std::string, size_t = 0);
	float term(std::string, size_t = 0);
	float factor(std::string, size_t = 0);
public:
	Expression(short padsCount) {
		this->padsCount = padsCount;
	}
	float parseExpression(std::string);
	float parseNumber(std::string, bool = false);
	float parseNumber(std::string, bool, int);
};

