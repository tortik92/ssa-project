#include "Parser.h"


int Parser::interpret(size_t len, std::string* code) {
    try {
        for (short i = 0; i < len; i++) {
            
            char* line;
            strcpy(line, code[i].c_str());
            char* openParenthesis = std::strchr(line, '('); // find '(' in the codeline 

            if (openParenthesis != nullptr) {
                // '(' found
                *openParenthesis = '\0'; // Null-terminate the second part
                char* parameters = openParenthesis + 1; // parameters is anything after '('
                
                // --- KEYWORDS ---
                if(std::strcmp(line, "if") == 0) {

                }
                else if (std::strcmp(line, "goto") == 0) {

                }
                else if (std::strcmp(line, "reset") == 0) {

                }

                // --- FUNCTIONS ---
                // *SPEAKER*
                else if (std::strcmp(line, "say") == 0) {

                }
                else if (std::strcmp(line, "playmusic_for_milliseconds") == 0) {

                }
                else if (std::strcmp(line, "alarm_all_active") == 0) {

                }
                // *WAIT*
                else if (std::strcmp(line, "wait") == 0) {

                }
                else if (std::strcmp(line, "wait_until_mats_occupied") == 0) {

                }
                // *MISC*
                else if (std::strcmp(line, "deactivate_random_active") == 0) {

                }
            }
            else {
                char* equalSign = std::strchr(line, '=');
                if (equalSign != nullptr) {
                    // '=' found
                    *equalSign = '\0';  // Null-terminate the second part
                    std::cout << "Keyword: " << line << "\n";
                    std::cout << "Parameters: " << equalSign + 1 << "\n";
                }

                throw "Expected '(' or '=' in line " + i;
            }
        }
    }
    catch (char errMsg[]) {
        ErrorHelper errorHelper;
        errorHelper.throwError(errMsg);
    }
    return 0;
}

