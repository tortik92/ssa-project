#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "Pad.h"

class Parser
{
private:
	// variables & objects
	unsigned short connectedPadsCount;
	unsigned short activePadsCount;
	
	Pad* connectedPads;
	Pad* activePads;

	// functions
	std::string* tokenize(std::string line);
	std::string stripComments(std::string line); // gets called by tokenize
	int parseNumber(std::string number, bool requiredPositive);
	int parseNumber(std::string number, bool requiredPositive, int maxValue); // calls parseNumber with 2 params (above)
	void parseIf(std::string args, std::string jmpTo);
public:
	Parser(unsigned short nrOfPads) {
		std::srand(std::time(nullptr));

		this->connectedPadsCount = nrOfPads;
		this->connectedPads = new Pad[nrOfPads];

		this->activePadsCount = nrOfPads;
		this->activePads = new Pad[nrOfPads]; // per default, all pads are active
	}
	int interpret(size_t len, std::string* code);
};

