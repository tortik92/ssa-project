#include "Parser.h"

const char* Parser::interpret(char code[codeLines][codeLineLen], int linesCount) {
    char errmsg[errmsgLen] = "";

    for (int i = 0; i < linesCount; i++) {
        char tokenizedLine[tokenizerSplits][tokenLength];

        if (tokenize(code[i], tokenizedLine) == noValueReturned) continue; // ignore lines that only have a comment in them
        else {
            // --- KEYWORDS ---
            if (tokenizedLine[0] == "if") { // IF
                char jmpTo[3] = "";
                char elseJmpTo[3] = "";
                strcpy_s(elseJmpTo, strstr(tokenizedLine[2], "else"));

                if (elseJmpTo != NULL) {
                    strncpy_s(jmpTo, tokenizedLine[2], strlen(tokenizedLine[2]) - strlen(elseJmpTo));
                }
                else {
                    strcpy_s(jmpTo, tokenizedLine[2]);
                    _itoa_s(i + 1, elseJmpTo, 10); // default: go to next line
                }

                short ifStatementResult = evaluateIfStatement(tokenizedLine[1], i + 1);

                if (errorOccurred(errmsg, ifStatementResult, i + 1)) {
                    return errmsg;
                } 
                else if (ifStatementResult) // if the statement in tokenizedLine[1] is true
                {
                    int newIndex = parseNumber(jmpTo, true, linesCount);

                    if (!errorOccurred(errmsg, newIndex, i + 1)) {
                        i = newIndex;
                    }
                    else {
                        return errmsg;
                    }

                    continue;
                }
                else {
                    int newIndex = parseNumber(elseJmpTo, true, linesCount);

                    if (!errorOccurred(errmsg, newIndex, i + 1)) {
                        i = newIndex;
                    }
                    else {
                        return errmsg;
                    }
                    continue;
                }
            }
            else if (tokenizedLine[0] == "goto") { // GOTO
                int newIndex = parseNumber(tokenizedLine[1], true, linesCount);

                if (!errorOccurred(errmsg, newIndex, i + 1)) {
                    i = newIndex;
                }
                else {
                    return errmsg;
                }
                continue;
            }
            else if (tokenizedLine[0] == "reset") { // RESET
                i = -1;
                continue;
            }

            // --- FUNCTIONS ---
            // *SPEAKER*
            else if (tokenizedLine[0] == "say") { // SAY
                for (int i = 0; i < padsCount; i++) {
                    pads[i].say(tokenizedLine[1]);
                }
            }
            else if (tokenizedLine[0] == "play_music") { // PLAY MUSIC
                for (int i = 0; i < padsCount; i++) {
                    bool mustBePositive = true;
                    int milliSeconds = parseNumber(tokenizedLine[1], mustBePositive, INT32_MAX);

                    if (errorOccurred(errmsg, milliSeconds, i + 1)) {
                        return errmsg;
                    }
                    else {
                        pads[i].play_music(milliSeconds);
                    }
                }
            }
            else if (tokenizedLine[0] == "alarm") { // ALARM
                for (int i = 0; i < padsCount; i++) {
                    pads[i].alarm();
                }
            }
            // *WAIT*
            else if (tokenizedLine[0] == "wait") { // WAIT
                bool mustBePositive = true;
                int millis = parseNumber(tokenizedLine[1], mustBePositive, INT32_MAX);
                if (errorOccurred(errmsg, millis, i + 1)) {
                    return errmsg;
                }
                else {
                    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(millis));
                }
            }
            else if (tokenizedLine[0] == "wait_until") { // WAIT UNTIL
                std::cout << "All pads occupied";
            }
            // *MISC*
            else if (tokenizedLine[0] == "activate") { // ACTIVATE
            
            }
            else if (tokenizedLine[0] == "activate_all") { // ACTIVATE ALL
                activateAllPads();
            }
            else if (tokenizedLine[0] == "deactivate") { // DEACTIVATE
                bool mustBePositive = true;
                short padToDeactivate = (short)parseNumber(tokenizedLine[1], mustBePositive, padsCount);

                if (!errorOccurred(errmsg, padToDeactivate, i + 1)) {
                    deactivatePad(padToDeactivate);
                    continue;
                }
                else {
                    return errmsg;
                }
            }
            else if (tokenizedLine[0] == "deactivate_all") { // DEACTIVATE ALL
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
    line = stripComments(line);

    if (strcmp(line, "") == 0) 
    {
        for (int i = 0; i < tokenizerSplits; i++) {
            strncpy_s(tokenizedLine[i], " ", tokenLength);
            tokenizedLine[i][tokenLength] = '\0';
        }

        return noValueReturned;
    }
    else {
        char* lineAfterOpenParent = strchr(line, '(');
        if (lineAfterOpenParent != NULL) {
            char* lineAfterCloseParent = strchr(line, ')');
            if (lineAfterCloseParent == NULL) {
                return noCloseParenthesis;
            }
            else if (&lineAfterOpenParent > &lineAfterCloseParent) { // if '(' comes after ')'
                return noOpenParenthesis;
            }
            else {
                strncpy_s(tokenizedLine[0], line, strlen(line) - strlen(lineAfterOpenParent) - 1);
                strcpy_s(tokenizedLine[1], lineAfterOpenParent);
                strcpy_s(tokenizedLine[2], lineAfterCloseParent);
                return noErrors;
            }
        }
        else {
            // '(' not found, try '='
            char* lineAfterEqualSign = strchr(line, '=');
            if (lineAfterEqualSign != NULL) {
                // --- EXPRESSION ---

                char* lineAfterVariableDeclaration = strchr(line, '$');
                if (lineAfterVariableDeclaration != NULL) {
                    int accessIndex = parseNumber(&lineAfterVariableDeclaration[1], true, SHRT_MAX);

                    setVariableContents(accessIndex, parseExpression(lineAfterEqualSign));

                    return noValueReturned;
                }
                else {
                    return invalidVariableAccess;
                }
            }


            // throw std::invalid_argument("Invalid statement (perhaps missing '(' or '=')");
        }

        return invalidStatement;
    }
}


/**
 * Removes comments from a code line.
 * 
 * \param line to search for comments
 * \return line with comments removed
 */
char* Parser::stripComments(char line[codeLineLen]) {
    char* nextToken;
    char* lineAfterHashtag = strtok_s(line, "#", &nextToken);

    if (lineAfterHashtag != NULL) {
        char lineBeforeHashtag[codeLineLen - 1];
        strncpy_s(lineBeforeHashtag, line, strlen(line) - strlen(lineAfterHashtag));

        return lineBeforeHashtag;
    }

    return line;
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
        parsedArgs[j] = parseExpression(args[j]);

        if (errorOccurred(errmsg, parsedArgs[i], lineNr)) {
            return parsedArgs[i];
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
    char* lineAfterOperator = strstr(statement, operatorToken);
    if (lineAfterOperator == NULL) { // operator not found
        return invalidComparisonOperator;
    }
    else {
        strncpy_s(args[0], strlen(statement) - strlen(lineAfterOperator - 1), lineAfterOperator, ifArgLen);
        strcpy_s(args[1], lineAfterOperator);
    }
}

int Parser::activatePad(short index) {
    throw std::exception("Not implemented");
}

int Parser::activateAllPads() {
    throw std::exception("Not implemented");
}

int Parser::deactivatePad(short index) {
    throw std::exception("Not implemented");
}

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
    
    if (errCode >= notANumber && errCode <= unknownKeyword) {
        foundAnError = true;

        strcpy_s(errmsg, errmsgLen, errorMessages[errCode + (notANumber * -1)]);
        char lineNote[11] = " (line )";
        const short numInsertPos = 7;
        char lineNrAsStr[3] = "";

        _itoa_s(lineNr, lineNrAsStr, 10);
        short lineNoteLen = (short)strlen(lineNote);
        short lineNrAsStrLen = (short)strlen(lineNrAsStr);

        for (short i = lineNoteLen; i >= numInsertPos; i--) {
            lineNote[i + lineNrAsStrLen] = lineNote[i];
        }
        for (short i = 0; i < lineNrAsStrLen; i++) {
            lineNote[numInsertPos + i] = lineNrAsStr[i];
        }
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
int Parser::parseNumber(char numberAsString[numberAsStrLen], bool mustBePositive, int maxAllowedValue) {
    // remove leading and trailing spaces/unwanted characters
    const char notLikedInNumbers[] = " \t\n\r\f\v";
    char strippedString[numberAsStrLen] = "";

    int stripStrIndex = 0;
    for (int i = 0; i < strlen(numberAsString); i++) {
        for (int j = 0; j < strlen(notLikedInNumbers); j++) {
            if (numberAsString[i] != notLikedInNumbers[j]) {
                strippedString[stripStrIndex] = numberAsString[i];
                stripStrIndex++;
            }
        }
    }

    // return macro
    if (numberAsString == "ACTIVE_COUNT") {
        return padsCount;
    }

    // return variable contents
    char* lineAfterDollarSign = strchr(numberAsString, '$');
    if (lineAfterDollarSign != NULL) {
        return getVariableContents((short)parseNumber(&lineAfterDollarSign[0], mustBePositive, maxAllowedValue));
    }

    // return random number
    char* lineAfterRandom = strstr(numberAsString, "random(");
    if (lineAfterRandom != NULL) {
        char* lineAfterCloseParent = strrchr(numberAsString, ')');
        if (lineAfterCloseParent != NULL) {
            char numberInPar[6] = "";
            strncpy_s(numberInPar, lineAfterRandom, &lineAfterRandom[0] - &lineAfterCloseParent[0]);

            int num = parseNumber(numberInPar, mustBePositive, maxAllowedValue);
            if (num < 0) {
                return valueNegative;
            }

            return num != 0 ? std::rand() % num : 0;
        }
        else {
            return noCloseParenthesis;
        }
    }

    // return parsed number
    else {
        int parsedNumber = 0;
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
}

float parseExpression(char expression[codeLineLen]) {
    float numbers[100];
    char operators[100];
    int numIndex = 0, opIndex = 0;

    for (int i = 0; i < codeLineLen; i++) {
        if (isdigit(expression[i])) {
            float num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;
            numbers[numIndex++] = num;
        }
        else if (expression[i] == '+' || expression[i] == '-') {
            while (opIndex > 0 && (operators[opIndex - 1] == '*' || operators[opIndex - 1] == '/')) {
                char op = operators[--opIndex];
                float b = numbers[--numIndex];
                float a = numbers[--numIndex];
                if (op == '*')
                    numbers[numIndex++] = a * b;
                else
                    numbers[numIndex++] = a / b;
            }
            operators[opIndex++] = expression[i];
        }
        else if (expression[i] == '*' || expression[i] == '/') {
            while (opIndex > 0 && (operators[opIndex - 1] == '*' || operators[opIndex - 1] == '/')) {
                char op = operators[--opIndex];
                float b = numbers[--numIndex];
                float a = numbers[--numIndex];
                if (op == '*')
                    numbers[numIndex++] = a * b;
                else
                    numbers[numIndex++] = a / b;
            }
            operators[opIndex++] = expression[i];
        }
    }

    while (opIndex > 0) {
        char op = operators[--opIndex];
        float b = numbers[--numIndex];
        float a = numbers[--numIndex];
        if (op == '+')
            numbers[numIndex++] = a + b;
        else
            numbers[numIndex++] = a - b;
    }

    return numbers[0];
}
