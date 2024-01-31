#include "Initializer.h"

void Initializer::onDataRecv(const unsigned int* mac, const unsigned char* incomingData, int len){
	// copy data into struct
    std::memcpy(&msg, incomingData, len);

    // validate file signature
    if (validateSignature(msg.signature)) {
        // call parser
        Parser parser(msg.padsCount);
        try {
            parser.interpret(CODE_BUFFER_LENGTH, msg.code);
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

bool Initializer::validateSignature(unsigned char fileSignature[3]) {
    // first three bytes should be 72, 65 and 74 (HAJ in ASCII) when decoded to uints
    if (fileSignature[0] == 'S' && 
        fileSignature[1] == 'D' && 
        fileSignature[2] == 'L' && 
        fileSignature[3] == 'P') {
        return true;
    }
    return false;
}