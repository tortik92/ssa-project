#include "Parser.h"


int Parser::interpret(size_t len, std::string* code) {
    try {
        for (short i = 0; i < len; i++) {
            std::string* tokenizedLine = tokenize(code[i]);

            // --- KEYWORDS ---
            if (tokenizedLine[0] == "if") {
                std::cout << "if keyword detected." << "\n";
            }
            else if (tokenizedLine[0] == "goto") {
                try {
                    int lineNr = std::stoi(tokenizedLine[1]);
                }
                catch (const std::invalid_argument& ia) {
                    throw std::invalid_argument("Expression must be a valid line number (line" + std::to_string(i) + ")");
                }
            }
            else if (tokenizedLine[0] == "reset") {
                std::cout << "resetting program." << "\n";
                std::cin.get();

                i = -1;
                continue;
            }

            // --- FUNCTIONS ---
            // *SPEAKER*
            else if (tokenizedLine[0] == "say") {

            }
            else if (tokenizedLine[0] == "playmusic_for_milliseconds") {

            }
            else if (tokenizedLine[0] == "alarm_all_active") {

            }
            // *WAIT*
            else if (tokenizedLine[0] == "wait") {

            }
            else if (tokenizedLine[0] == "wait_until_mats_occupied") {

            }
            // *MISC*
            else if (tokenizedLine[0] == "deactivate_random_active") {

            }
            // *EXPRESSION*
            else {

            }
        }
    }
    catch (char errMsg[]) {
        ErrorHelper errorHelper;
        errorHelper.throwError(errMsg);
    }
    return 0;
}

std::string* Parser::tokenize(std::string line) {
    // check for '('
    size_t openParentLoc = line.find('(');
    if (openParentLoc != std::string::npos) {
        // '(' found
        // 
        // check for ')'
        size_t closeParentLoc = line.find(')');
        if (closeParentLoc == std::string::npos) {
            throw "Missing \')\'";
        }
        else {
            std::string tmp[] = { line.substr(0, openParentLoc), line.substr(openParentLoc, closeParentLoc) };
            return tmp;
        }
    }
    else {
        // '(' not found, try '='
        size_t equalSignPos = line.find('=');
        if (equalSignPos != std::string::npos) {
            // '=' found

            std::string tmp[] = {line.substr(0, equalSignPos), line.substr(equalSignPos)};
            return tmp;
        }

        throw "Expected '(' or '='";
    }
}

