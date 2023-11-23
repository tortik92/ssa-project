#include "InputStream.h"

const char* InputStream::getFileLocation() { // May be changed later on
    /* read filename
    std::cout << "Please input a filename: ";
    std::string filePath;
    std::cin >> filePath;

    return filePath;
    */
    return "test.txt";
}


void InputStream::readFile() {
    const char* filePath = getFileLocation();
    std::ifstream inputFile(filePath); // create stream

    try {
        if (!inputFile.is_open()) {
            throw "No file with specified file name found."; // File not found
        }
        else {
            // read first line and check for data
            std::string firstLine;
            getline(inputFile, firstLine);

            const char* firstLineCharArray = firstLine.c_str();
            for (int i = 0; i < firstLine.length(); i++) {
                if (!std::isxdigit(firstLineCharArray[i])) {
                    throw "The gameplay procedure file does not have the correct signature.";
                }
            }


            unsigned short fileSignature[3] = {
                (unsigned short)std::stoi(firstLine.substr(0, 2), 0, 16),
                (unsigned short)std::stoi(firstLine.substr(2, 2), 0, 16),
                (unsigned short)std::stoi(firstLine.substr(4, 2), 0, 16)
            };

            if (validateFileSignature(fileSignature)) {
                // determine length of file
                std::string line;
                int nrOfLines = 0;
                while (getline(inputFile, line)) {
                    
                    nrOfLines++;
                }

                // Reset the file line[1] to read the file again
                inputFile.clear();
                inputFile.seekg(0);

                // Skip first line (file signature)
                getline(inputFile, firstLine);

                // read data from file and store it into array
                std::string* inputArray = new std::string[nrOfLines];
                int index = 0;
                while (getline(inputFile, inputArray[index]) && index < nrOfLines) {
                    index++;
                }
                inputFile.close(); // clean up

                // call interpret
                Parser parser;
                parser.interpret(nrOfLines, inputArray);

                delete[] inputArray; // always remember to clean up afterwards!
            }
            else {
                inputFile.close();
                throw "The gameplay procedure file does not have the correct signature.";
            }
        }
    }
    catch (char errMsg[]) {
        ErrorHelper errorHelper;

        if (errMsg != NULL) {
            errorHelper.throwError(errMsg);
        }
        else {
            errorHelper.throwError("An unknown error occurred.");
            inputFile.close();
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