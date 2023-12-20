#include "Parser.h"


int Parser::interpret(size_t len, std::string* code) {
    for (short i = 0; i < len; i++) {
        try {
            std::string* tokenizedLine = tokenize(code[i]);
            if (tokenizedLine == nullptr) continue; // ignore lines that are commented out
            else {
                // --- KEYWORDS ---
                if (tokenizedLine[0] == "if") {
                    std::string jmpTo, elseJmpTo;
                    size_t elsePos = tokenizedLine[2].find(" else ");

                    if (elsePos != std::string::npos) { // 'else' exists
                        jmpTo = tokenizedLine[2].substr(0, elsePos);
                        elseJmpTo = tokenizedLine[2].substr(elsePos + 6, tokenizedLine[2].length() - elsePos - 6);
                    }
                    else { // else does not exist
                        jmpTo = tokenizedLine[2];
                        elseJmpTo = std::to_string(i + 1); // go to next line
                    }

                    if (parseIfStatement(tokenizedLine[1])) // if the statement in tokenizedLine[1] is true
                    {
                        i = parseGotoStatement(jmpTo, (short)len);
                        std::cout << "Going to line " + i;
                        continue;
                    }
                    else {
                        i = parseGotoStatement(elseJmpTo, (short)len);
                        std::cout << "Going to line " + i;
                        continue;
                    }
                }
                else if (tokenizedLine[0] == "goto") {
                    i = parseGotoStatement(tokenizedLine[1], (int)len);
                    std::cout << "Going to line " + i;
                    continue;
                }
                else if (tokenizedLine[0] == "reset") {
                    i = -1;
                    std::cout << "Resetting program";
                    continue;
                }

                // --- FUNCTIONS ---
                // *SPEAKER*
                else if (tokenizedLine[0] == "say") {
                    for (int i = 0; i < padsCount; i++) {
                        pads[i].say(tokenizedLine[1]);
                    }
                }
                else if (tokenizedLine[0] == "play_music") {
                    for (int i = 0; i < padsCount; i++) {
                        pads[i].play_music(parseNumber(tokenizedLine[1]));
                    }
                }
                else if (tokenizedLine[0] == "alarm") {
                    for (int i = 0; i < padsCount; i++) {
                        pads[i].alarm();
                    }
                }
                // *WAIT*
                else if (tokenizedLine[0] == "wait") {
                    std::cout << "Waiting for " << tokenizedLine[1] << " ms";
                    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(parseNumber(tokenizedLine[1], true)));
                }
                else if (tokenizedLine[0] == "wait_until_pads_occupied") {
                    std::cout << "All pads occupied";
                }
                // *MISC*
                else if (tokenizedLine[0] == "deactivate") {
                    short padToDeactivate = parseNumber(tokenizedLine[1], true, padsCount);
                    deactivateActivePad(padToDeactivate);
                    std::cout << "Deactivated pad " << padToDeactivate;
                }
                // *EXPRESSION*
                else {

                }
            }
        }
        catch (const std::exception &e) {
            // Add line number to exception, then rethrow with new error message
            std::string errMsg = std::string(e.what()) + " (line " + std::to_string(i + 1) + ")";
            throw std::exception(errMsg.c_str());
        }
    }
    return 0;
}

std::string* Parser::tokenize(std::string line) {
    std::string* tokenizedLine = new std::string[3];
    line = stripComments(line);

    if (line == "") {
        return nullptr;
    }
    else {
        // check for '('
        size_t openParentLoc = line.find('(');
        if (openParentLoc != std::string::npos) {
            // '(' found
            // check for ')'
            size_t closeParentLoc = line.rfind(')');
            if (closeParentLoc == std::string::npos) {
                throw std::invalid_argument("Expected ')'");
            }
            else if (openParentLoc > closeParentLoc) {
                throw std::invalid_argument("Expected '('");
            }
            else {
                tokenizedLine[0] = line.substr(0, openParentLoc);
                tokenizedLine[1] = line.substr(openParentLoc + 1, closeParentLoc - openParentLoc - 1);
                tokenizedLine[2] = line.substr(closeParentLoc + 1);
                return tokenizedLine;
            }
        }
        else {
            // '(' not found, try '='
            size_t equalSignPos = line.find('=');
            if (equalSignPos != std::string::npos) {
                // '=' found

                std::string tmp[] = { line.substr(0, equalSignPos), line.substr(equalSignPos) };
                return tmp;
            }

            throw std::invalid_argument("Invalid statement (perhaps missing '(' or '=')");
        }
    }
}

std::string Parser::stripComments(std::string line) {
    // check for '#'
    size_t hashtagLoc = line.find('#');
    if (hashtagLoc != std::string::npos) {
        return line.substr(0, hashtagLoc);
    }

    // if no comment found, return line
    return line;
}

int Parser::parseNumber(std::string numberAsString, bool requiredPositive = false) {
    int parsedNumber = 0;

    // remove leading and trailing spaces/unwanted characters
    const char notLikedInNumbers[] = " \t\n\r\f\v";
    numberAsString.erase(0, numberAsString.find_first_not_of(notLikedInNumbers)); 
    numberAsString.erase(numberAsString.find_last_not_of(notLikedInNumbers) + 1);
    
    // return macro
    if (numberAsString == "ACTIVE_COUNT"){
        return padsCount;
    }
    // return random number
    size_t randomPos = numberAsString.find("random(");
    if (randomPos != std::string::npos) {
        size_t closeParentPos = numberAsString.rfind(")");
        if (closeParentPos != std::string::npos) {
            // extract number from random method
            std::string numberInRandomPar = numberAsString.substr(randomPos + 7, closeParentPos - randomPos - 7);

            // make random number
            parsedNumber = std::rand() % parseNumber(numberInRandomPar, true);
        }
        else {
            throw std::invalid_argument("Missing ')'");
        }
    }
    else {
        try {
            parsedNumber = std::stoi(numberAsString);
        }
        catch (const std::invalid_argument) {
            throw std::invalid_argument("Expression must be a number");
        }

        if (requiredPositive && parsedNumber < 0) {
            throw std::invalid_argument("Expression must be a positive number");
        }
    }

    return parsedNumber;
}

int Parser::parseNumber(std::string number, bool requiredPositive, int maxValue) {
    int parsedNumber = parseNumber(number, requiredPositive);
    
    if (parsedNumber > maxValue) {
        throw std::invalid_argument(std::string("Expression must be a number smaller than " + maxValue).c_str());
    }

    return parsedNumber;
}

bool Parser::parseIfStatement(std::string condition) {
    if (condition.find(" == ") != std::string::npos) {
        std::string* args = parseOperator(condition, " == ");
        return parseNumber(args[0]) == parseNumber(args[1]);
    }
    else if (condition.find(" < ") != std::string::npos) {
        std::string* args = parseOperator(condition, " < ");
        return parseNumber(args[0]) < parseNumber(args[1]);
    }
    else if (condition.find(" <= ") != std::string::npos) {
        std::string* args = parseOperator(condition, " <= ");
        return parseNumber(args[0]) <= parseNumber(args[1]);
    }
    else if (condition.find(" > ") != std::string::npos) {
        std::string* args = parseOperator(condition, " > ");
        return parseNumber(args[0]) > parseNumber(args[1]);
    }
    else if (condition.find(" >= ") != std::string::npos) {
        std::string* args = parseOperator(condition, " >= ");
        return parseNumber(args[0]) >= parseNumber(args[1]);
    }
    else if (condition.find(" != ") != std::string::npos) {
        std::string* args = parseOperator(condition, " != ");
        return parseNumber(args[0]) != parseNumber(args[1]);
    }
    else{
        throw std::invalid_argument("Invalid comparison operator in if-statement (missing spaces before or after operator?)");
    }
}

short Parser::parseGotoStatement(std::string jmpTo, short fileLength) {
    return parseNumber(jmpTo, true, fileLength) - 1;
}

std::string* Parser::parseOperator(std::string statement, std::string operatorToken) {
    std::string args[2];

    size_t operatorPos = statement.find(operatorToken);
    if (operatorPos == std::string::npos) {
        return nullptr;
    }
    else {
        args[0] = statement.substr(0, operatorPos);
        args[1] = statement.substr(operatorPos + operatorToken.length(), statement.length() - operatorPos - operatorToken.length());

        return args;
    }
}

void Parser::deactivateActivePad(short index) {
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