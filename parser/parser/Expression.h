#pragma once

#include <iostream>
#include <string>


class Expression
{
private:
	// variables & objects
	unsigned short padsCount;

	static constexpr short variablesLength = 8;
	int variables[variablesLength];

	int expression(std::string, size_t = 0);
	int term(std::string, size_t = 0);
	int factor(std::string, size_t = 0);
public:
	Expression(short padsCount) {
		this->padsCount = padsCount;
		for (short i = 0; i < variablesLength; i++) {
			variables[i] = 0;
		}
	}
	int parseExpression(std::string);
	int parseNumber(std::string, bool = false);
	int parseNumber(std::string, bool, int);

	unsigned short getPadsCount() {
		return this->padsCount;
	}
	void setPadsCount(unsigned short padsCount) {
		this->padsCount = padsCount;
	}
	int getVariable(short accessIndex) {
		return variables[accessIndex];
	}
	void setVariable(short accessIndex, int value) {
		this->variables[accessIndex] = value;
	}
	short getVariablesLength() {
		return variablesLength;
	}
};

