#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "Expression.h"
#include "Pad.h"

// array lengths
constexpr int codeLines = 25;
constexpr int codeLineLen = 50;

constexpr int variablesCount = 8;

constexpr int tokenizerSplits = 3;
constexpr int tokenLength = 40;

constexpr int ifArgCount = 2;
constexpr int ifArgLen = 30;
constexpr int comparisonOperatorLength = 3;

constexpr int numberAsStrLen = 25;

constexpr int errmsgLen = 40;


// ---error codes---
// no errors occurred
constexpr int noErrors = 109;
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

constexpr int errmsgTooBig = -118;
constexpr int invalidStatement = -119;
constexpr int unknownKeyword = -120;

class Parser
{
private:
	// variables & objects
	unsigned short padsCount;
	int variables[variablesCount];
	
	Expression expr;
	Pad* pads;

	// helper functions
	int tokenize(char line[codeLineLen], char tokenizedLine[tokenizerSplits][tokenLength]);
	char* stripComments(char line[codeLineLen]);
	int parseOperator(char line[codeLineLen], const char operatorToken[comparisonOperatorLength], char args[ifArgCount][ifArgLen]);
	int evaluateIfStatement(char condition[codeLineLen], short lineNr);
	int parseNumber(char numberAsString[numberAsStrLen], bool mustBePositive, int maxAllowedValue);
	float parseExpression(char expression[codeLineLen]);

	// pads interaction
	int activatePad(short index);
	int activateAllPads();
	int deactivatePad(short index);
	int deactivateAllPads();

	bool errorOccurred(char errmsg[errmsgLen], int errCode, short line);

	// numbers
	int getVariableContents(short accessIndex) {
		if (accessIndex < 0 || accessIndex > variablesCount) {
			return invalidVariableAccess;
		}
		else {
			return variables[accessIndex];
		}
	}
	int setVariableContents(short accessIndex, int value) {
		if (accessIndex < 0 || accessIndex > variablesCount) {
			return invalidVariableAccess;
		}
		else {
			variables[accessIndex] = value;
			return noErrors;
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
	const char* interpret(char code[codeLines][codeLineLen], int linesCount);
};

