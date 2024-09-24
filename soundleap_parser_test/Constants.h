#pragma once

#include <stddef.h>

// raw data length
constexpr int signatureLen = 5;
constexpr size_t codeBufferLen = 400;

// array lengths
constexpr int linesCount = 25;
constexpr size_t codeLineLen = 50;

constexpr int variablesCount = 8;

constexpr int tokenizerSplits = 3;
constexpr size_t tokLen = 40;

constexpr int ifArgCount = 2;
constexpr size_t ifArgLen = 30;
constexpr size_t compOperatorLen = 3;

constexpr size_t shortAsStrLen = 7;
constexpr size_t intAsStrLen = 12;

constexpr size_t errmsgLen = 40;

constexpr size_t padCommandLen = 6;
constexpr size_t macAddrLen = 6;
constexpr size_t maxSupportedPads = 8;
constexpr size_t maxAllowedTextToSpeechLen = 128;


// ---error codes----
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
constexpr int indexOutOfRange = -122;

constexpr int invalidStatement = -119;
constexpr int unknownKeyword = -120;
constexpr int invalidValue = -121;


class Constants {
};