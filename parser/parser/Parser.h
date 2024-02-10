#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
#include <memory>

#include "Expression.h"
#include "Pad.h"

class Parser
{
private:
	// variables & objects
	unsigned short padsCount;
	
	Expression expression = NULL;
	Pad* pads;

	// parsing
	std::string* tokenize(std::string line);

	// helper functions
	std::string stripComments(std::string line);
	std::unique_ptr<std::string[]> parseOperator(std::string line, const char operatorToken[2]);

	bool parseIfStatement(std::string condition);
	short parseGotoStatement(std::string jmpTo, short fileLength);

	// other
	void deactivateActivePad(short index);
public:
	Parser(unsigned short padsCount) {
		std::srand((unsigned int)std::time(nullptr));

		this->padsCount = padsCount;

		Expression tmpExpr(padsCount);
		this->expression = tmpExpr;
		this->pads = new Pad[padsCount];
	}
	~Parser() {
		delete[] pads;
	}
	void interpret(std::vector<std::string> code);
};

