#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <random>

#include "Expression.h"
#include "Pad.h"

// array lengths
#define CODE_LINES 25
#define CODE_LINE_LEN 50

#define VARIABLES_COUNT 8

#define TOKENIZER_SPLITS 3
#define TOKEN_LENGTH 40

#define IF_ARG_COUNT 2
#define IF_ARG_LEN 30

// ---error codes---
// no errors occurred
#define NO_ERRORS 0
#define NO_VALUE_RETURNED 1

// number value errors
#define ERR_NOT_A_NUMBER -1
#define ERR_VALUE_NEGATIVE -2
#define ERR_VALUE_OUT_OF_RANGE -3

// things not not found
#define ERR_INVALID_COMPARISON_OPERATOR -4
#define ERR_NO_OPEN_PARENTHESIS -5
#define ERR_NO_CLOSE_PARENTHESIS -6
#define ERR_INVALID_VARIABLE_ACCESS -666

class Parser
{
private:
	// variables & objects
	unsigned short padsCount;
	int variables[VARIABLES_COUNT];
	
	Expression expr;
	Pad* pads;

	// helper functions
	short tokenize(char* line, char tokenizedLine[TOKENIZER_SPLITS][TOKEN_LENGTH]);
	char* stripComments(char line[CODE_LINE_LEN]);
	short parseOperator(char* line, const char operatorToken[2], char args[IF_ARG_COUNT][IF_ARG_LEN]);
	short parseIfStatement(char* condition);
	short parseGotoStatement(char* jmpTo, short fileLength);
	int parseNumber(char* numberAsString);

	// pads interaction
	short activatePad(short index);
	short activateAllPads();
	short deactivatePad(short index);
	short deactivateAllPads();

	void buildErrorMsg(char* errmsg, char* error, int insertPos, int line);

	// numbers
	int getVariableContents(short accessIndex) {
		if (accessIndex < 0 || accessIndex > VARIABLES_COUNT) {
			return ERR_INVALID_VARIABLE_ACCESS;
		}
		else {
			return variables[accessIndex];
		}
	}
	int setVariableContents(short accessIndex, int value) {
		if (accessIndex < 0 || accessIndex > VARIABLES_COUNT) {
			return ERR_INVALID_VARIABLE_ACCESS;
		}
		else {
			variables[accessIndex] = value;
			return NO_ERRORS;
		}
	}
public:
	Parser(unsigned short padsCount) {
		// to use random
		std::srand((unsigned int)std::time(nullptr));

		this->padsCount = padsCount;
		this->pads = new Pad[padsCount];
	}
	~Parser() {
		delete[] pads;
	}

	// entry/main function
	const char* interpret(char code[CODE_LINES][CODE_LINE_LEN], int linesCount);
};

