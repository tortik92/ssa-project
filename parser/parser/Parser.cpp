#include "Parser.h"

/**
 * Interprets the code.
 * 
 * \param code The code that should be executed
 * \return How the debugging of the program went
 */
const char* Parser::interpret(char code[linesCount][codeLineLen], char errmsg[errmsgLen]) {
    for (int i = 0; i < linesCount; i++) {
        char tokenizedLine[tokenizerSplits][tokenLength];

        int tokenizerReturnCode = tokenize(code[i], tokenizedLine);
        if (errorOccurred(errmsg, tokenizerReturnCode, i + 1)) return errmsg;
        if (tokenizerReturnCode == noValueReturned) continue; // ignore lines that only have a comment in them
        else {
            // --- KEYWORDS ---
            if (strcmp(tokenizedLine[0], "if") == 0) { // IF
                char jmpTo[3] = "";
                char* elseJmpTo = strstr(tokenizedLine[2], "else");

                if (elseJmpTo != NULL) {
                    strncpy_s(jmpTo, tokenizedLine[2], strlen(tokenizedLine[2]) - strlen(elseJmpTo));
                }
                else {
                    strcpy_s(jmpTo, tokenizedLine[2]);
                    strh.toString(i + 1, elseJmpTo, 3); // default: go to next line
                }

                short ifStatementResult = evaluateIfStatement(tokenizedLine[1], i + 1);

                if (errorOccurred(errmsg, ifStatementResult, i + 1)) {
                    return errmsg;
                } 
                else if (ifStatementResult) // if the statement in tokenizedLine[1] is true
                {
                    int newIndex = (int)parseExpression(jmpTo, true, linesCount);

                    if (!errorOccurred(errmsg, newIndex, i + 1)) {
                        i = newIndex;
                    }
                    else {
                        return errmsg;
                    }

                    continue;
                }
                else {
                    int newIndex = (int)parseExpression(elseJmpTo, true, linesCount);

                    if (!errorOccurred(errmsg, newIndex, i + 1)) {
                        i = newIndex;
                    }
                    else {
                        return errmsg;
                    }
                    continue;
                }
            }
            else if (strcmp(tokenizedLine[0], "goto") == 0) { // GOTO
                int newIndex = (int)parseExpression(tokenizedLine[1], true, linesCount);

                if (!errorOccurred(errmsg, newIndex, i + 1)) {
                    i = newIndex;
                }
                else {
                    return errmsg;
                }
                continue;
            }
            // --- FUNCTIONS ---
            // *SPEAKER*
            else if (strcmp(tokenizedLine[0], "say") == 0) { // SAY
                for (int i = 0; i < padsCount; i++) {
                    pads[i].say(tokenizedLine[1]);
                }
            }
            else if (strcmp(tokenizedLine[0], "play_music") == 0) { // PLAY MUSIC
                for (int i = 0; i < padsCount; i++) {
                    int milliSeconds = (int)parseExpression(tokenizedLine[1], true, INT32_MAX);

                    if (errorOccurred(errmsg, milliSeconds, i + 1)) {
                        return errmsg;
                    }
                    else {
                        pads[i].play_music(milliSeconds);
                    }
                }
            }
            else if (strcmp(tokenizedLine[0], "alarm") == 0) { // ALARM
                for (int i = 0; i < padsCount; i++) {
                    pads[i].alarm();
                }
            }
            // *WAIT*
            else if (strcmp(tokenizedLine[0], "wait") == 0) { // WAIT
                int millis = (int)parseExpression(tokenizedLine[1], true, INT32_MAX);
                if (errorOccurred(errmsg, millis, i + 1)) {
                    return errmsg;
                }
                else {
                    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(millis));
                }
            }
            else if (strcmp(tokenizedLine[0], "wait_until") == 0) { // WAIT UNTIL
                std::cout << "All pads occupied";
            }
            // *MISC*
            else if (strcmp(tokenizedLine[0], "activate") == 0) { // ACTIVATE
            
            }
            else if (strcmp(tokenizedLine[0], "activate_all") == 0) { // ACTIVATE ALL
                activateAllPads();
            }
            else if (strcmp(tokenizedLine[0], "deactivate") == 0) { // DEACTIVATE
                bool mustBePositive = true;
                short padToDeactivate = (short)parseExpression(tokenizedLine[1], mustBePositive, padsCount);

                if (!errorOccurred(errmsg, padToDeactivate, i + 1)) {
                    deactivatePad(padToDeactivate);
                    continue;
                }
                else {
                    return errmsg;
                }
            }
            else if (strcmp(tokenizedLine[0], "deactivate_all") == 0) { // DEACTIVATE ALL
                deactivateAllPads();
            }
            else {
                errorOccurred(errmsg, unknownKeyword, i + 1);
                return errmsg;
            }
        }
    }

    return "Compiled successfully.";
}

/**
 * Looks for '(' and splits the code line into 3 tokens, if '(' is not found it searches for '=' and tries to parse an expression.
 * 
 * \param line line of code to be tokenized
 * \param tokenizedLine where the tokens are stored, will be overwritten
 * \return noErrors or noValueReturned (tokenizedLine will be empty) if successful
 * \throw noCloseParenthesis, noOpenParenthesis, invalidVariableAccess and invalidStatement
 */
int Parser::tokenize(char line[codeLineLen], char tokenizedLine[tokenizerSplits][tokenLength]) {
    stripComments(line);

    if (strcmp(line, "") == 0) 
    {
        for (int i = 0; i < tokenizerSplits; i++) {
            strncpy_s(tokenizedLine[i], " ", tokenLength);
        }

        return noValueReturned;
    }
    else {
        char* openParentLoc = strchr(line, '(');
        if (openParentLoc != NULL) {
            char* closeParentLoc = strchr(line, ')');
            if (closeParentLoc == NULL) {
                return noCloseParenthesis;
            }
            else if (&openParentLoc > &closeParentLoc) { // if '(' comes after ')'
                return noOpenParenthesis;
            }
            else {
                strncpy_s(tokenizedLine[0], line, strlen(line) - strlen(openParentLoc));
                strncpy_s(tokenizedLine[1], openParentLoc + 1, strlen(openParentLoc) - strlen(closeParentLoc) - 1);
                strcpy_s(tokenizedLine[2], closeParentLoc + 1);
                return success;
            }
        }
        else {
            // '(' not found, try '='
            char* lineAfterEqualSign = strchr(line, '=');
            if (lineAfterEqualSign != NULL) {
                // --- EXPRESSION ---

                char* lineAfterVariableDeclaration = strchr(line, '$');
                if (lineAfterVariableDeclaration != NULL) {
                    int accessIndex = (int)parseNumber(&lineAfterVariableDeclaration[1], true, SHRT_MAX);

                    setVariableContents(accessIndex, parseExpression(lineAfterEqualSign, false, INT32_MAX));

                    return noValueReturned;
                }
                else return invalidVariableAccess;
            }
        }

        return invalidStatement;
    }
}


/**
 * Removes comments from a code line.
 * 
 * \param line to search for comments
 */
void Parser::stripComments(char line[codeLineLen]) {
    char* hashtagLoc = strchr(line, '#');

    while (hashtagLoc != NULL && *hashtagLoc != '\0') {
        *hashtagLoc = '\0';
        hashtagLoc++;
    }
}

/**
 * Parses the expressions on both sides of the comparison operator.
 * 
 * \param condition the condition found in the parentheses of the 'if'-statement
 * \param lineNr current line number
 * \return 0 if false, 1 if true
 * \throw every error that can occur when parsing expressions + invalidComparisonOperator
 */
int Parser::evaluateIfStatement(char condition[codeLineLen], short lineNr) {
    char comparisonOperators[][comparisonOperatorLength] = {"==", "<=", "<", ">=", ">", "!="};
    char args[ifArgCount][ifArgLen] = { {0} };
    int parsedArgs[ifArgCount] = { 0, 0 };
    char errmsg[errmsgLen] = "";

    int i = 0;
    for(i = 0; i < sizeof(comparisonOperators); i++) {
        if (strstr(condition, comparisonOperators[i]) != NULL) {
            parseOperator(condition, comparisonOperators[i], args);
            break;
        }
    }


    for (int j = 0; j < ifArgCount; j++) {
        parsedArgs[j] = parseExpression(args[j], false, INT32_MAX);

        if (errorOccurred(errmsg, (int)parsedArgs[i], lineNr)) {
            return (int)parsedArgs[i];
        }
    }

    switch (i) {
    case 0:
        return parsedArgs[0] == parsedArgs[1];
    case 1:
        return parsedArgs[0] <= parsedArgs[1];
    case 2:
        return parsedArgs[0] < parsedArgs[1];
    case 3:
        return parsedArgs[0] >= parsedArgs[1];
    case 4:
        return parsedArgs[0] > parsedArgs[1];
    case 5:
        return parsedArgs[0] != parsedArgs[1];
    default:
        return invalidComparisonOperator;
    }
}

int Parser::parseOperator(char statement[codeLineLen], const char operatorToken[comparisonOperatorLength], char args[ifArgCount][ifArgLen]) {
    char* operatorPos = strstr(statement, operatorToken);
    if (operatorPos == NULL) { // operator not found
        return invalidComparisonOperator;
    }
    else {
        int beforeOperatorLen = strlen(statement) - strlen(operatorPos);
        if (beforeOperatorLen > ifArgLen) {
            return valueTooLarge;
        }
        else {
            strncpy_s(args[0], ifArgLen, statement, beforeOperatorLen);
            strcpy_s(args[1], operatorPos + strlen(operatorToken));
            return success;
        }
    }
}

// TODO: implement
int Parser::activatePad(short index) {
    throw std::exception("Not implemented");
}

// TODO: implement
int Parser::activateAllPads() {
    throw std::exception("Not implemented");
}

// TODO: implement
int Parser::deactivatePad(short index) {
    throw std::exception("Not implemented");
}

// TODO: implement
int Parser::deactivateAllPads() {
    throw std::exception("Not implemented");
}

/**
 * Converts the error code into the appropriate error message.
 * 
 * \param errmsg location to put the error message into
 * \param errCode the error code, must correspond to the constexpr error codes
 * \param lineNr where the error occurred
 * \return if an error message was built
 */
bool Parser::errorOccurred(char errmsg[errmsgLen], int errCode, short lineNr) {
    bool foundAnError = false;
    const char errorMessages[][30] = {
        "Not a number",
        "Value must not be negative",
        "Value out of range",
        "Invalid comparison operator",
        "No '(' found",
        "No ')' found",
        "Invalid variable access",
        "Message too big",
        "Invalid statement",
        "Unknown keyword"
    };
    
    if (errCode >= unknownKeyword && errCode <= notANumber) {
        foundAnError = true;

        int accessIndex = errCode * -1 + notANumber;
        strcpy_s(errmsg, errmsgLen, errorMessages[accessIndex]);

        const short lineNoteLen = 11;
        const short numInsertPos = 7;
        char lineNote[lineNoteLen] = " (line )";
        char lineNrAsStr[5] = { 0 };

        strh.toString(lineNr, lineNrAsStr, 3);
        strh.insert(lineNote, lineNoteLen, numInsertPos, lineNrAsStr);

        strcat_s(errmsg, errmsgLen, (const char*)lineNote);
    }

    return foundAnError;
}

/**
 * Parses strings and special tokens into integers.
 * 
 * \param numberAsString the number as a char array
 * \param mustBePositive if this is set to true, the function returns valueNegative if the value is negative
 * \param maxAllowedValue checks if the parsed value exceeds the limit, returns valueOutOfRange if applicable
 * \return parsed number if everything went fine
 * \throw valueNegative, noCloseParenthesis (for randoms), valueOutOfRange, notANumber
 */
int Parser::parseNumber(char numberAsString[intAsStrLen], bool mustBePositive, int maxAllowedValue) {
    int parsedNumber = 0;
    while (*numberAsString)
    {
        if ((*numberAsString >= '0') && (*numberAsString <= '9'))
            return notANumber;
        numberAsString++;
        parsedNumber++;
    }

    try {
        parsedNumber = std::stoi(numberAsString);

        if (mustBePositive && parsedNumber < 0) {
            return valueNegative;
        }
        if (parsedNumber > maxAllowedValue) {
            return valueOutOfRange;
        }
    }
    catch (const std::invalid_argument) {
        return notANumber;
    }

    return parsedNumber;
}

int Parser::parseExpression(char expression[codeLineLen], bool mustBePositive, int maxAllowedValue) {
    // remove leading and trailing spaces/unwanted characters
    const char notLikedInNumbers[] = " \t\n\r\f\v";
    char strippedString[codeLineLen] = "";

    int stripStrIndex = 0;
    bool faultyCharacterFound = false;
    for (int i = 0; i < strlen(expression); i++) {
        for (int j = 0; j < strlen(notLikedInNumbers); j++) {
            if (expression[i] == notLikedInNumbers[j]) {
                faultyCharacterFound = true;
            }
        }

        if (!faultyCharacterFound) {
            strippedString[stripStrIndex] = expression[i];
            stripStrIndex++;
        } // else continue with next
    }

    // macro
    strh.replaceNum(strippedString, codeLineLen, "PADS_COUNT", padsCount);

    // variable contents
    char* dollarPos = strchr(strippedString, '$');
    while (dollarPos != NULL && *dollarPos + 1 != '\0') {
        int variableContent = getVariableContents((short)parseNumber(&dollarPos[1], true, variablesCount));
        char toReplace[] = { *dollarPos, *dollarPos + 1, '\0'};
        strh.replaceNum(dollarPos, codeLineLen - (strippedString - dollarPos), toReplace, variableContent);
    }

    // random number
    char* randomPos = strstr(strippedString, "random(");
    if (randomPos != NULL) {
        char* lineAfterCloseParent = strrchr(strippedString, ')');
        if (lineAfterCloseParent != NULL) {
            char numberInPar[shortAsStrLen] = "";
            strncpy_s(numberInPar, randomPos + 7, &randomPos[0] - &lineAfterCloseParent[0]);

            int num = parseExpression(numberInPar, mustBePositive, maxAllowedValue);
            if (num < 0) {
                return valueNegative;
            }

            char toReplace[9 + shortAsStrLen] = "random()";
            strh.insert(toReplace, 9 + shortAsStrLen, 7, numberInPar);

            strh.replaceNum(strippedString, codeLineLen, toReplace, (int)(num != 0 ? std::rand() % num : 0));
        }
        else {
            return noCloseParenthesis;
        }
    }

    // parse the expression
    int numbers[100] = { 0 };
    char operators[100] = { 0 };
    int numIndex = 0, opIndex = 0;

    for (int i = 0; i < codeLineLen; i++) {
        if (isdigit(strippedString[i])) {
            int num = 0;
            while (isdigit(strippedString[i])) {
                num = num * 10 + (strippedString[i] - '0');
                i++;
            }
            i--;
            numbers[numIndex++] = num;
        }
        else if (strippedString[i] == '+' || strippedString[i] == '-') {
            while (opIndex > 0 && (operators[opIndex - 1] == '*' || operators[opIndex - 1] == '/')) {
                char op = operators[--opIndex];
                int b = numbers[--numIndex];
                int a = numbers[--numIndex];
                if (op == '*')
                    numbers[numIndex++] = a * b;
                else
                    numbers[numIndex++] = a / b;
            }
            operators[opIndex++] = strippedString[i];
        }
        else if (strippedString[i] == '*' || strippedString[i] == '/') {
            while (opIndex > 0 && (operators[opIndex - 1] == '*' || operators[opIndex - 1] == '/')) {
                char op = operators[--opIndex];
                int b = numbers[--numIndex];
                int a = numbers[--numIndex];
                if (op == '*')
                    numbers[numIndex++] = a * b;
                else
                    numbers[numIndex++] = a / b;
            }
            operators[opIndex++] = strippedString[i];
        }
    }

    while (opIndex > 0) {
        char op = operators[--opIndex];
        int b = numbers[--numIndex];
        int a = numbers[--numIndex];
        if (op == '+')
            numbers[numIndex++] = a + b;
        else
            numbers[numIndex++] = a - b;
    }

    return numbers[0];
}
