#include "Initializer.h"

void Initializer::onDataRecv(const unsigned int* mac, const char incomingData[CODE_BUFFER_LENGTH], int len) {
    // Copy signature
    strncpy_s(reinterpret_cast<char*>(msg.signature), SIGNATURE_LENGTH + 1, incomingData, SIGNATURE_LENGTH);
    msg.signature[SIGNATURE_LENGTH] = '\0';  // Null-terminate the signature

    // validate file signature
    if (validateSignature(msg.signature)) {
        // Extract padsCount
        sscanf_s(incomingData + SIGNATURE_LENGTH, "%2d", &msg.padsCount);

        // Extract code
        const char* codePos = std::strchr(incomingData, '\n');
        if (codePos != nullptr) {
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, codePos + 1);  // +1 to skip the '\n'
        }
        else {
            // Handle error: code not found
            strcpy_s(msg.code, CODE_BUFFER_LENGTH, "");
        }

        Parser parser(msg.padsCount);
        try {
            // parse char[] to vector<string>
            std::string codeAsString(msg.code);
            std::vector<std::string> codeLines;

            size_t newLinePos = 0;
            while ((newLinePos = codeAsString.find("\n")) != std::string::npos) {
                codeLines.push_back(codeAsString.substr(0, newLinePos));
                codeAsString.erase(0, newLinePos + 1);
            }

            // add last line
            codeLines.push_back(codeAsString);

            parser.interpret(codeLines);
        }
        catch (const std::invalid_argument& ia) {
            throw ia;
        }
        catch (const std::exception& e) {
            std::cout << e.what() << "\n";
        }
    }
    else {
        throw std::invalid_argument("Invalid signature");
    }
}

bool Initializer::validateSignature(unsigned char fileSignature[SIGNATURE_LENGTH]) {
    if (fileSignature[0] == 'S' && 
        fileSignature[1] == 'D' && 
        fileSignature[2] == 'L' && 
        fileSignature[3] == 'P') {
        return true;
    }
    return false;
}