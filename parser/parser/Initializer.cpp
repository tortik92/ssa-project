#include "Initializer.h"

void Initializer::onDataRecv(const unsigned int* mac, char incomingData[CODE_BUFFER_LENGTH], int len) {
    // Copy signature
    strncpy_s(msg.signature, SIGNATURE_LENGTH + 1, incomingData, SIGNATURE_LENGTH);
    msg.signature[SIGNATURE_LENGTH] = '\0';  // Null-terminate the signature

    if (validateSignature(msg.signature)) {
        // Extract padsCount
        sscanf_s(incomingData + SIGNATURE_LENGTH, "%2d", &msg.padsCount);

        // Extract code
        const char* codePos = strchr(incomingData, '\n');
        if (codePos != nullptr) {
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, codePos + 1);  // +1 to skip the '\n'
        }
        else {
            // Handle error: code not found
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, "");
        }

        Parser parser(msg.padsCount);

        char** codeLines;

        // split the code based on newline characters
        const char* delim = "\n";
        char* next_token;
        char* token = strtok_s(incomingData, delim, &next_token);

        while (token != NULL) {
            std::cout << token;

            token = strtok_s(NULL, delim, &next_token);
        }


        // parser.interpret(codeLines);
    }
    else {
        throw std::invalid_argument("Invalid signature");
    }
}

bool Initializer::validateSignature(char fileSignature[SIGNATURE_LENGTH]) {
    if (fileSignature[0] == 'S' && 
        fileSignature[1] == 'D' && 
        fileSignature[2] == 'L' && 
        fileSignature[3] == 'P') {
        return true;
    }
    return false;
}