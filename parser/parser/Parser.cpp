#include "Parser.h"


int Parser::interpret(size_t len, std::string* code) {
    for (short i = 0; i < len; i++) {
        try {
            std::string* tokenizedLine = tokenize(code[i]);
            if (tokenizedLine == nullptr) { // ignore lines that are commented out
                continue;
            }
            else {
                // --- KEYWORDS ---
                if (tokenizedLine[0] == "if") {
                    std::cout << "if keyword detected." << "\n";
                }
                else if (tokenizedLine[0] == "goto") {
                    short lineNr = parseNumber(tokenizedLine[1], true, (int)len);
                    i = lineNr - 2;
                    continue;
                }
                else if (tokenizedLine[0] == "reset") {
                    std::cout << "resetting program." << "\n";
                    std::cin.get(); // wait for user input

                    i = -1;
                    continue;
                }

                // --- FUNCTIONS ---
                // *SPEAKER*
                else if (tokenizedLine[0] == "say") {
                    for (int i = 0; i < activePads; i++) {
                        pads[i].say(tokenizedLine[1]);
                    }
                }
                else if (tokenizedLine[0] == "play_music") {
                    for (int i = 0; i < activePads; i++) {
                        pads[i].say(tokenizedLine[1]);
                    }
                }
                else if (tokenizedLine[0] == "alarm") {
                    for (int i = 0; i < activePads; i++) {
                        pads[i].alarm();
                    }
                }
                // *WAIT*
                else if (tokenizedLine[0] == "wait") {
                    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(parseNumber(tokenizedLine[1], true)));
                }
                else if (tokenizedLine[0] == "wait_until_mats_occupied") {

                }
                // *MISC*
                else if (tokenizedLine[0] == "deactivate") {

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
            size_t closeParentLoc = line.find(')');
            if (closeParentLoc == std::string::npos) {
                throw std::invalid_argument("Missing ')'");
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

int Parser::parseNumber(std::string number, bool requiredPositive) {
    int parsedNumber = 0;

    try {
        parsedNumber = std::stoi(number);
    }
    catch (const std::invalid_argument& ia) {
        throw std::invalid_argument("Expression must be a number");
    }

    if (requiredPositive && parsedNumber < 0) {
        throw std::invalid_argument("Expression must be a positive number");
    }

    return parsedNumber;
}

int Parser::parseNumber(std::string number, bool requiredPositive, int maxValue) {
    int parsedNumber = parseNumber(number, requiredPositive);
    
    if (parsedNumber > maxValue) {
        throw std::invalid_argument("Expression must be a number smaller than the maximum value");
    }

    return parsedNumber;
}

void Parser::parseIf(std::string args, std::string jmpTo) {
    std::string comparisonOperators[] = { " == ", " < ", " <= ", " > ", " >= ", " != "};

    // go through each available operator
    int foundOperatorIndex = -1;
    size_t operatorPos = std::string::npos;
    for (int i = 0; i < comparisonOperators->size(); i++) {
        operatorPos = args.find(comparisonOperators[i]);

        if (operatorPos != std::string::npos) {
            foundOperatorIndex = i;
            break;
        }
    }

    // chose operation depending on result
    switch (foundOperatorIndex)
    {
    case 0: // equal to

        break;
    case 1: // greater than

        break;
    case 2: // greater or equal

        break;
    case 3: // less than

        break;
    case 4: // less or equal

        break;
    case 5: // not equal

        break;
    default:
        throw std::invalid_argument("Invalid comparison operator in if-statement (missing spaces before or after operator?)");
    }
}