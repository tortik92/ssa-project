#include "Parser.h"

const char* Parser::interpret(char code[CODE_LINES][CODE_LINE_LEN], int linesCount) {
    for (int i = 0; i < linesCount; i++) {
        char tokenizedLine[TOKENIZER_SPLITS][TOKEN_LENGTH];

        if (tokenize(code[i], tokenizedLine) == NO_VALUE_RETURNED) continue; // ignore lines that only have a comment in them
        else {
            // --- KEYWORDS ---
            if (tokenizedLine[0] == "if") { // IF
                char jmpTo[3] = "00";
                char elseJmpTo[3] = "00";
                strcpy_s(elseJmpTo, strstr(tokenizedLine[2], "else"));

                if (elseJmpTo != NULL) { // 'else' exists
                    strncpy_s(jmpTo, tokenizedLine[2], strlen(tokenizedLine[2]) - strlen(elseJmpTo));
                }
                else { // else does not exist
                    strcpy_s(jmpTo, tokenizedLine[2]);
                    itoa(i + 1, elseJmpTo, 10); // default: go to next line
                }

                short ifStatementResult = parseIfStatement(tokenizedLine[1]);
                if (ifStatementResult != ERR_INVALID_COMPARISON_OPERATOR) {
                    if (parseIfStatement(tokenizedLine[1])) // if the statement in tokenizedLine[1] is true
                    {
                        i = parseGotoStatement(jmpTo, (short)linesCount);
                        continue;
                    }
                    else {
                        i = parseGotoStatement(elseJmpTo, (short)linesCount);
                        continue;
                    }
                }
                else {
                    char errmsg[] = "Invalid comparison operator in if statement";
                    return buildErrorMsg(errmsg, );
                        (char*)" (line " + std::to_string(i + 1) + ")";
                }
            }
            else if (tokenizedLine[0] == "goto") { // GOTO
                i = parseGotoStatement(tokenizedLine[1], (short)linesCount);
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
                    pads[i].play_music(parseNumber(tokenizedLine[1]));
                }
            }
            else if (tokenizedLine[0] == "alarm") { // ALARM
                for (int i = 0; i < padsCount; i++) {
                    pads[i].alarm();
                }
            }
            // *WAIT*
            else if (tokenizedLine[0] == "wait") { // WAIT
                int millis = parseNumber(tokenizedLine[1]);
                if (millis < 0) {

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
            
            }
            else if (tokenizedLine[0] == "deactivate") { // DEACTIVATE
                short padToDeactivate = (short)parseNumber(tokenizedLine[1], true, padsCount);
                deactivatePad(padToDeactivate);
            }
            else if (tokenizedLine[0] == "deactivate_all") { // DEACTIVATE ALL
                deactivateAllPads();
            }
            else {
                char errmsg[50] = "Unknown keyword \"\"";
                int insertPos = 17; //               ^ Insertion here
                int lineNr = i + 1;
                buildErrorMsg(errmsg, tokenizedLine[0], insertPos, lineNr);
                return errmsg;
            }
        }


        try {
            
        }
        catch (const std::exception &e) {
            // Add line number to exception, then rethrow with new error message
            std::string errMsg = std::string(e.what()) + ;
            throw std::exception(errMsg.c_str());
        }
    }

    return "Compiled successfully.";
}

short Parser::tokenize(char line[CODE_LINE_LEN], char tokenizedLine[TOKENIZER_SPLITS][TOKEN_LENGTH]) {
    line = stripComments(line);

    if (strcmp(line, "") == 0) 
    {
        for (int i = 0; i < TOKENIZER_SPLITS; i++) {
            strncpy_s(tokenizedLine[i], " ", TOKEN_LENGTH);
            tokenizedLine[i][TOKEN_LENGTH] = '\0'; // null-terminate
        }

        return NO_ERRORS;
    }
    else {
        // check for '('
        char* lineAfterOpenParent = strchr(line, '(');
        if (lineAfterOpenParent != NULL) {
            // '(' found, check for ')'
            char* lineAfterCloseParent = strchr(line, ')');
            if (lineAfterCloseParent == NULL) {
                return ERR_NO_CLOSE_PARENTHESIS;
            }
            else if (&lineAfterOpenParent > &lineAfterCloseParent) { // if '(' comes after ')'
                return ERR_NO_OPEN_PARENTHESIS;
            }
            else {
                strncpy_s(tokenizedLine[0], line, strlen(line) - strlen(lineAfterOpenParent) - 1);
                strcpy_s(tokenizedLine[1], lineAfterOpenParent);
                strcpy_s(tokenizedLine[2], lineAfterCloseParent);
                return NO_ERRORS;
            }
        }
        else {
            // '(' not found, try '='
            char* lineAfterEqualSign = strchr(line, '=');
            if (lineAfterEqualSign != NULL) {
                // '=' found
                // --- EXPRESSION ---

                char* lineAfterVariableDeclaration = strchr(line, '$');
                if (lineAfterVariableDeclaration != NULL) {
                    short accessIndex = (short)parseNumber(&lineAfterVariableDeclaration[1]);

                    setVariableContents(accessIndex, parseExpression(lineAfterEqualSign));

                    return NO_VALUE_RETURNED; // ignore line
                }
                else {
                    throw std::invalid_argument("Missing variable to assign to");
                }
            }

            throw std::invalid_argument("Invalid statement (perhaps missing '(' or '=')");
        }
    }
}

char* Parser::stripComments(char* line) {
    char* nextToken;
    char* lineAfterHashtag = strtok_s(line, "#", &nextToken);

    // check for '#'
    if (lineAfterHashtag != NULL) {
        char* lineBeforeHashtag;
        strncpy_s()

        return lineAfterHashtag; // everything after hashtag has no meaning for us
    }

    // if no comment found, return line
    return line;
}

short Parser::parseIfStatement(char* condition) {
    char comparisonOperators[][3] = {"==", "!<", "<=", "!>", ">=", "!="};
    char args[IF_ARG_COUNT][IF_ARG_LEN] = { {0} };

    int i = 0;
    for(i = 0; i < sizeof(comparisonOperators); i++) {
        if (strstr(condition, comparisonOperators[i]) != NULL) {
            parseOperator(condition, comparisonOperators[i], args);

            break; // if found, break the loop
        }
    }

    switch (i) {
    case 0:
        return parseNumber(args[0]) == parseNumber(args[1]);
    case 1:
        return parseNumber(args[0]) < parseNumber(args[1]);
    case 2:
        return parseNumber(args[0]) <= parseNumber(args[1]);
    case 3:
        return parseNumber(args[0]) > parseNumber(args[1]);
    case 4:
        return parseNumber(args[0]) >= parseNumber(args[1]);
    case 5:
        return parseNumber(args[0]) != parseNumber(args[1]);
    default:
        return ERR_INVALID_COMPARISON_OPERATOR;
    }
}

short Parser::parseGotoStatement(char* jmpTo, short fileLength) {
    short jumpPos = (short)parseNumber(jmpTo);

    if (jumpPos < 0) {
        return ERR_VALUE_NEGATIVE;
    }
    else if (jumpPos > fileLength) {
        return ERR_VALUE_OUT_OF_RANGE;
    }
    else {
        return jumpPos;
    }
}

short Parser::parseOperator(char* statement, const char operatorToken[2], char args[IF_ARG_COUNT][IF_ARG_LEN]) {
    char* lineAfterOperator = strstr(statement, operatorToken);
    if (lineAfterOperator == NULL) { // operator not found
        return ERR_INVALID_COMPARISON_OPERATOR;
    }
    else {
        strncpy_s(args[0], strlen(statement) - strlen(lineAfterOperator - 1), lineAfterOperator, IF_ARG_LEN);
        strcpy_s(args[1], lineAfterOperator);
    }
}

short Parser::activatePad(short index) {
    throw std::exception("Not implemented");
}

short Parser::activateAllPads() {
    throw std::exception("Not implemented");
}

short Parser::deactivatePad(short index) {
    Pad* tmpArray = new Pad[padsCount - 1];
    // move everything up to the index to tmpArray
    std::copy(pads, pads + index, tmpArray);

    // move everything starting from the index + 1 to tmpArray
    std::copy(pads + index + 1, pads + padsCount, tmpArray); 

    // delete old and store in new
    delete[] pads;
    pads = tmpArray;
    
    padsCount--;
}

short Parser::deactivateAllPads() {
    throw std::exception("Not implemented");
}

void Parser::buildErrorMsg(char* errmsg, char* error, int insertPos, int line) {
    int errmsg_len = strlen(errmsg);
    int error_len = strlen(error);

    // make space for the error string
    for (int i = errmsg_len; i >= insertPos; i--) {
        errmsg[i + error_len] = errmsg[i];
    }

    // copy the error string into the error message string
    for (int i = 0; i < error_len; i++) {
        errmsg[insertPos + i] = error[i];
    }
}

int Parser::parseNumber(char* numberAsString) {
    // remove leading and trailing spaces/unwanted characters
    const char notLikedInNumbers[] = " \t\n\r\f\v";

    throw std::exception("Not implemented"); // not implemented
    for (int i = 0; i < strlen(numberAsString); i++) {
        for (int j = 0; j < strlen(notLikedInNumbers); j++) {
            if (numberAsString[i] == notLikedInNumbers[j]) {
                
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
        return getVariableContents((short)parseNumber(&lineAfterDollarSign[0]));
    }

    // return random number
    char* lineAfterRandom = strstr(numberAsString, "random(");
    if (lineAfterRandom != NULL) {
        char* lineAfterCloseParent = strrchr(numberAsString, ')');
        if (lineAfterCloseParent != NULL) {
            // extract number from random method
            char numberInPar[6] = "";
            strncpy_s(numberInPar, lineAfterRandom, &lineAfterRandom[0] - &lineAfterCloseParent[0]);

            // make random number
            int num = parseNumber(numberInPar);
            if (num < 0) {
                return ERR_VALUE_NEGATIVE;
            }

            return num != 0 ? std::rand() % num : 0;
        }
        else {
            return ERR_NO_CLOSE_PARENTHESIS;
        }
    }

    // return parsed number
    else {
        int parsedNumber = 0;
        try {
            parsedNumber = std::stoi(numberAsString);
        }
        catch (const std::invalid_argument) {
            return ERR_NOT_A_NUMBER;
        }

        return parsedNumber;
    }
}