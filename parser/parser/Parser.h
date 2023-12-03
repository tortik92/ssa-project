#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "Pad.h"
#include "GameVariables.h"

class Parser
{
private:
	unsigned short connectedPads;
	unsigned short activePads;
	Pad* pads;

	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
	int parseNumber(std::string number, bool requiredPositive);
	int parseNumber(std::string number, bool requiredPositive, int maxValue); // calls parseNumber with 2 params (above)
	void parseIf(std::string args, std::string jmpTo);
public:
	Parser(unsigned short connectedPads) {
		this->connectedPads = connectedPads;
		this->activePads = activePads; // per default, all pads are active
		this->pads = new Pad[connectedPads];
	}
	int interpret(size_t len, std::string* code);
};

