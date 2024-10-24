#include <sys/_stdint.h>
#pragma once

#include <string>
#include <iostream>

#include <Arduino.h>

#include "StringHelper.h"
#include "Constants.h"
#include "Pad.h"

class Parser {
private:
  // variables & objects
  unsigned short padsCount;
  int variables[variablesCount];

  Pad* pads[maxSupportedPads];

  // helper functions
  int tokenize(char line[codeLineLen], char tokenizedLine[tokenizerSplits][tokLen]);
  void stripComments(char line[codeLineLen]);
  int parseOperator(char line[codeLineLen], const char operatorToken[compOperatorLen], char args[ifArgCount][ifArgLen]);
  int parseIf(char condition[codeLineLen], short lineNr);
  int parseNumber(char numberAsString[intAsStrLen], bool mustBePositive, int maxAllowedValue);
  int parseExpression(char expression[codeLineLen], bool mustBePositive, int maxAllowedValue);

  // pads interaction
  int activatePad(short index);
  void activateAllPads();
  int deactivatePad(short index);
  void deactivateAllPads();

  bool errorOccurred(char errmsg[errmsgLen], int errCode, short line);

  // variables
  int getVariableContents(const short accessIndex) {
    if (accessIndex < 0 || accessIndex > variablesCount) {
      return invalidVariableAccess;
    } else {
      return variables[accessIndex];
    }
  }
  int setVariableContents(const short accessIndex, const int value) {
    if (accessIndex < 0 || accessIndex > variablesCount) {
      return invalidVariableAccess;
    } else {
      variables[accessIndex] = value;
      return success;
    }
  }
public:
  Parser(unsigned short padsCount, uint8_t macAddresses[maxSupportedPads][macAddrLen]) {
    this->padsCount = padsCount;

    for (int i = 0; i < variablesCount; i++) {
      this->variables[i] = 0;
    }

    for(size_t i = 0; i < padsCount && i < maxSupportedPads; i++) {
      pads[i] = new Pad(macAddresses[i]);
    }
  }

  // entry/main function
  const char* interpret(char code[linesCount][codeLineLen], char errmsg[errmsgLen]);
};
