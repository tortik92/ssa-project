#include "InputStream.h"

const char* InputStream::getFileLocation() { // May be changed later on
    /* read filename
    std::cout << "Please input a filename: ";
    std::string filePath;
    std::cin >> filePath;

    return filePath;
    */
    return "Reise_nach_Jerusalem.txt";
}


void InputStream::readFile() {
    const char* filePath = getFileLocation();
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
                    throw 2;
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

                // Reset the file parameters to read the file again
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
                throw 2;
            }
        }
    }
    catch (int err) {
        ErrorHelper errorHelper;

        switch (err) {
        case 1: // File not found 
            errorHelper.reportError("No file with specified file name found.");
            inputFile.close();
            break;
        case 2: // Invalid File signature
            errorHelper.reportError("The gameplay procedure file does not have the correct signature.");
            inputFile.close();
            break;

        default:
            errorHelper.reportError("An unknown error occurred.");
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