#include "InputStream.h"

void InputStream::read() {
    getFileLocation();
}

void InputStream::getFileLocation() { // May be changed later on
    // read filename
    std::cout << "Please input a filename: ";
    std::string filePath;
    std::cin >> filePath;

    readFile(filePath);
}


void InputStream::readFile(std::string filePath) {

    std::ifstream inputFile(filePath); // create stream

    try {
        if (!inputFile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read first line and check for data
            std::string firstLine;
            getline(inputFile, firstLine);

            const char* firstLineCharArray = firstLine.c_str();
            for (int i = 0; i < firstLine.length(); i++) {
                if (!std::isxdigit(firstLineCharArray[i])) {
                    throw 3;
                }
            }
            delete[] firstLineCharArray;


            unsigned short fileSignature[3] = {
                (short)std::stoi(firstLine.substr(0, 2), 0, 16),
                (short)std::stoi(firstLine.substr(2, 2), 0, 16),
                (short)std::stoi(firstLine.substr(4, 2), 0, 16)
            };

            if (validateFileSignature(fileSignature)) {
                // determine length of file
                int nrOfLines = 0;
                while (!inputFile.eof()) {
                    nrOfLines++;
                }
                inputFile.close();

                // read data from file and store it into array
                inputFile.open(filePath);
                std::string* inputArray = new std::string[nrOfLines];

                while (!inputFile.eof()) {
                    getline(inputFile, inputArray[nrOfLines]);
                }

                inputFile.close(); // clean up

                // call interpret
                Parser parser;
                parser.interpret(inputArray->size(), inputArray);
            }
            else {
                inputFile.close();
                throw 3;
            }
        }
    }
    catch (int err) {
        switch (err) {
        case 1: // File not found
            std::cout << "[ERROR]: No file \"" + filePath + "\" found.";
            inputFile.close();
            break;
        case 2: // File too long
            std::cout << "[ERROR]: The file \"" + filePath + "\" is longer than stated in the first parameter of the file.";
            inputFile.close();
            break;
        case 3: // Invalid File signature
            std::cout << "[ERROR]: The file \"" + filePath + "\" does not have the correct signature.";
            inputFile.close();
            break;

        default:
            std::cout << "[ERROR]: An unknown error occurred.";
            inputFile.close();
            break;
        }
    }
}

bool InputStream::validateFileSignature(unsigned short fileSignature[3]) {
    // first three bytes should be 72, 65 and 74 (HAJ in ASCII) when decoded to uints
    if (fileSignature[0] == 72 && fileSignature[1] == 65 && fileSignature[2] == 74) {
        return true;
    }
    return false;
}