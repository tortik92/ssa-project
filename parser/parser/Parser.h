#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "StringHelper.h"
#include "Pad.h"

// array lengths
constexpr int linesCount = 25;
constexpr int codeLineLen = 50;
 
constexpr int variablesCount = 8;

constexpr int tokenizerSplits = 3;
constexpr int tokenLength = 40;

constexpr int ifArgCount = 2;
constexpr int ifArgLen = 30;
constexpr int comparisonOperatorLength = 3;

constexpr int shortAsStrLen = 5;
constexpr int intAsStrLen = 12;

constexpr int errmsgLen = 40;


// ---error codes---
// no errors occurred
constexpr int success = 109;
constexpr int noValueReturned = 110;

// number value errors
constexpr int notANumber = -111;
constexpr int valueNegative = -112;
constexpr int valueOutOfRange = -113;

// things not not found
constexpr int invalidComparisonOperator = -114;
constexpr int noOpenParenthesis = -115;
constexpr int noCloseParenthesis = -116;
constexpr int invalidVariableAccess = -117;

constexpr int valueTooLarge = -118;
constexpr int invalidStatement = -119;
constexpr int unknownKeyword = -120;

class Parser
{
private:
	// variables & objects
	unsigned short padsCount;
	int variables[variablesCount];
	
	StringHelper strh;
	Pad* pads;

	// helper functions
	int tokenize(char line[codeLineLen], char tokenizedLine[tokenizerSplits][tokenLength]);
	void stripComments(char line[codeLineLen]);
	int parseOperator(char line[codeLineLen], const char operatorToken[comparisonOperatorLength], char args[ifArgCount][ifArgLen]);
	int evaluateIfStatement(char condition[codeLineLen], short lineNr);
	int parseNumber(char numberAsString[intAsStrLen], bool mustBePositive, int maxAllowedValue);
	int parseExpression(char expression[codeLineLen], bool mustBePositive, int maxAllowedValue);

	// pads interaction
	int activatePad(short index);
	int activateAllPads();
	int deactivatePad(short index);
	int deactivateAllPads();

	bool errorOccurred(char errmsg[errmsgLen], int errCode, short line);

	// variables
	int getVariableContents(const short accessIndex) {
		if (accessIndex < 0 || accessIndex > variablesCount) {
			return invalidVariableAccess;
		}
		else {
			return variables[accessIndex];
		}
	}
	int setVariableContents(const short accessIndex, const int value) {
		if (accessIndex < 0 || accessIndex > variablesCount) {
			return invalidVariableAccess;
		}
		else {
			variables[accessIndex] = value;
			return success;
		}
	}
public:
	Parser(unsigned short padsCount) {
		// to use random
		std::srand((unsigned int)std::time(nullptr));

		this->padsCount = padsCount;
		
		for (int i = 0; i < variablesCount; i++) {
			this->variables[i] = 0;
		}

		this->pads = new Pad[padsCount];
	}
	~Parser() {
		delete[] pads;
	}

	// entry/main function
	const char* interpret(char code[linesCount][codeLineLen], char errmsg[errmsgLen]);
};

