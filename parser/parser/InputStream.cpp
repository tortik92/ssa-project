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
            throw "No file with specified file name found"; // File not found
        }
        else {
            // read first line and check for data
            std::string firstLine;
            getline(inputFile, firstLine);

            if (firstLine.length() != FIRST_LINE_LENGTH) {
                throw std::invalid_argument("Invalid first line length");
            }

            unsigned short fileSignature[3] = {
                (unsigned short)std::stoi(firstLine.substr(0, 2), 0, 16),
                (unsigned short)std::stoi(firstLine.substr(2, 2), 0, 16),
                (unsigned short)std::stoi(firstLine.substr(4, 2), 0, 16)
            };

            unsigned short padsCount = std::stoi(firstLine.substr(6, 1));

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

                // call the parser
                Parser parser(padsCount);
                try {
                    parser.interpret(nrOfLines, inputArray);
                }
                catch (const std::invalid_argument& ia) {
                    throw ia;
                }

                delete[] inputArray; // always remember to clean up afterwards!
            }
            else {
                inputFile.close();
                throw std::invalid_argument("Invalid file signature.");
            }
        }
    }
    catch (std::invalid_argument &ia) {
        throw ia;
    }
    catch (std::exception& e) {
        throw e;
    }
}

bool InputStream::validateFileSignature(unsigned short fileSignature[3]) {
    // first three bytes should be 72, 65 and 74 (HAJ in ASCII) when decoded to uints
    if (fileSignature[0] == 72 && fileSignature[1] == 65 && fileSignature[2] == 74) {
        return true;
    }
    return false;
}