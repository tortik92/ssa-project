#include "Initializer.h"

void Initializer::onDataRecv(const unsigned int* mac, char incomingData[CODE_BUFFER_LENGTH], int len) {
    strncpy_s(msg.signature, SIGNATURE_LENGTH + 1, incomingData, SIGNATURE_LENGTH);
    msg.signature[SIGNATURE_LENGTH] = '\0';

    if (validSignature(msg.signature)) {
        // Extract padsCount
        sscanf_s(incomingData + SIGNATURE_LENGTH, "%2d", &msg.padsCount);

        // Extract code
        const char* codePos = strchr(incomingData, '\n');
        if (codePos != nullptr) {
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, codePos + 1);
        }
        else {
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, "");
        }

        Parser parser(msg.padsCount);

        char code[linesCount][codeLineLen];

        // split the code based on newline characters
        char* next_token = NULL;
        char* token = strtok_s(msg.code, "\n", &next_token);

        
        for (int i = 0; i < linesCount; i++) {
            if (token != NULL) {
                strcpy_s(code[i], token);

                token = strtok_s(NULL, "\n", &next_token);
            }
            else {
                strcpy_s(code[i], "\0");
            }
        }

        char errmsg[40];
        parser.interpret(code, errmsg);

        std::cout << errmsg << "\n";
    }
    else {
        throw std::invalid_argument("Invalid signature");
    }
}

bool Initializer::validSignature(char fileSignature[SIGNATURE_LENGTH]) {
    if (fileSignature[0] == 'S' && 
        fileSignature[1] == 'D' && 
        fileSignature[2] == 'L' && 
        fileSignature[3] == 'P') {
        return true;
    }
    return false;
}