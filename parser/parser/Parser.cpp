#include "Parser.h"

void Parser::interpret(std::vector<std::string> code) {
    for (int i = 0; i < code.size(); i++) {
        try {
            std::array<std::string, 3> tokenizedLine = tokenize(code[i]);
            if (tokenizedLine[0] == "") continue; // ignore lines that are commented out
            else {
                // --- KEYWORDS ---
                if (tokenizedLine[0] == "if") {
                    std::string jmpTo, elseJmpTo;
                    size_t elsePos = tokenizedLine[2].find("else");

                    if (elsePos != std::string::npos) { // 'else' exists
                        jmpTo = tokenizedLine[2].substr(0, elsePos);
                        elseJmpTo = tokenizedLine[2].substr(elsePos + 4, tokenizedLine[2].length() - elsePos - 4);
                    }
                    else { // else does not exist
                        jmpTo = tokenizedLine[2];
                        elseJmpTo = std::to_string(i + 1); // go to next line
                    }

                    if (parseIfStatement(tokenizedLine[1])) // if the statement in tokenizedLine[1] is true
                    {
                        i = parseGotoStatement(jmpTo, (short)code.size());
                        continue;
                    }
                    else {
                        i = parseGotoStatement(elseJmpTo, (short)code.size());
                        continue;
                    }
                }
                else if (tokenizedLine[0] == "goto") {
                    i = parseGotoStatement(tokenizedLine[1], (short)code.size());
                    continue;
                }
                else if (tokenizedLine[0] == "reset") {
                    i = -1;
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
                        pads[i].play_music(expression.parseNumber(tokenizedLine[1]));
                    }
                }
                else if (tokenizedLine[0] == "alarm") {
                    for (int i = 0; i < padsCount; i++) {
                        pads[i].alarm();
                    }
                }
                // *WAIT*
                else if (tokenizedLine[0] == "wait") {
                    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(expression.parseNumber(tokenizedLine[1], true)));
                }
                else if (tokenizedLine[0] == "wait_until_pads_occupied") {
                    std::cout << "All pads occupied";
                }
                // *MISC*
                else if (tokenizedLine[0] == "deactivate") {
                    short padToDeactivate = (short)expression.parseNumber(tokenizedLine[1], true, padsCount);
                    deactivateActivePad(padToDeactivate);
                }
                else {
                    throw std::invalid_argument("Unknown keyword \"" + tokenizedLine[0] + "\"");
                }
            }
        }
        catch (const std::exception &e) {
            // Add line number to exception, then rethrow with new error message
            std::string errMsg = std::string(e.what()) + " (line " + std::to_string(i + 1) + ")";
            throw std::exception(errMsg.c_str());
        }
    }
}

std::array<std::string, 3> Parser::tokenize(std::string line) {
    std::array<std::string, 3> tokenizedLine;
    line = stripComments(line);

    if (line == "") {
        tokenizedLine.fill("");
        return tokenizedLine;
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
                // --- EXPRESSION ---

                size_t variableDeclarationPos = line.rfind('$', equalSignPos);
                if (variableDeclarationPos != std::string::npos) {
                    short accessIndex = 0;
                    try
                    {
                        accessIndex = (short)expression.parseNumber(line.substr(variableDeclarationPos + 1, 1), true, expression.getVariablesLength());
                    }
                    catch (const std::invalid_argument)
                    {
                        throw std::invalid_argument("Invalid variable name");
                    }

                    expression.setVariable(accessIndex, expression.parseExpression(line.substr(equalSignPos + 1, line.length() - equalSignPos)));

                    tokenizedLine.fill("");
                    return tokenizedLine; // ignore line
                }
                else {
                    throw std::invalid_argument("Missing variable to assign to");
                }
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



bool Parser::parseIfStatement(std::string condition) {
    if (condition.find("==") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, "==");
        return expression.parseNumber(args[0]) == expression.parseNumber(args[1]);
    }
    else if (condition.find("!<") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, "!<");
        return expression.parseNumber(args[0]) < expression.parseNumber(args[1]);
    }
    else if (condition.find("<=") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, "<=");
        return expression.parseNumber(args[0]) <= expression.parseNumber(args[1]);
    }
    else if (condition.find(">") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, "!>");
        return expression.parseNumber(args[0]) > expression.parseNumber(args[1]);
    }
    else if (condition.find(">=") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, ">=");
        return expression.parseNumber(args[0]) >= expression.parseNumber(args[1]);
    }
    else if (condition.find("!=") != std::string::npos) {
        std::unique_ptr<std::string[]> args = parseOperator(condition, "!=");
        return expression.parseNumber(args[0]) != expression.parseNumber(args[1]);
    }
    else{
        throw std::invalid_argument("Invalid comparison operator in if-statement");
    }
}

short Parser::parseGotoStatement(std::string jmpTo, short fileLength) {
    try {
        return (short)expression.parseNumber(jmpTo, true, fileLength) ;
    }
    catch (const std::invalid_argument) {
        throw std::invalid_argument("Invalid goto location");
    }
}

std::unique_ptr<std::string[]> Parser::parseOperator(std::string statement, const char operatorToken[2]) {
    std::unique_ptr<std::string[]> args(new std::string[2]);

    size_t operatorPos = statement.find(operatorToken);
    if (operatorPos == std::string::npos) {
        return nullptr;
    }
    else {
        args[0] = statement.substr(0, operatorPos);
        args[1] = statement.substr(operatorPos + 2, statement.length() - operatorPos - 2);

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
    expression.setPadsCount(expression.getPadsCount() - 1);
}