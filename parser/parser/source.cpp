#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::string filePath = getFileLocation();
    std::string* code = readFile(filePath);

}

std::string getFileLocation() { // May be changed later on
    // read filename
    std::cout << "Please input a filename: ";
    std::string filePath;
    std::cin >> filePath;

    return filePath;
}


std::string* readFile(std::string filePath) {
    
    std::ifstream inputFile(filePath); // create stream

    try {
        if (!inputFile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read first line and check for data
            std::string firstLine;
            getline(inputFile, firstLine);

            unsigned short fileSignature[3] = { 
                std::stoi(firstLine.substr(0, 2), 0, 16), 
                std::stoi(firstLine.substr(2, 2), 0, 16), 
                std::stoi(firstLine.substr(4, 2), 0, 16) 
            };

            if (validateFileSignature(fileSignature)) {
                // read data from file and store it into array
                int nrOfLines = 0;

                std::string* inputArray = new std::string[nrOfLines];

                int arrayindex = 0;
                while (!inputFile.eof()) {
                    getline(inputFile, inputArray[nrOfLines]);
                }

                inputFile.close(); // clean up
                return inputArray;
            } 
            else {
                throw 3;
            }
            
        }
    }
    catch (int err) {
        switch (err) {
        case 1: // File not found
            std::cout << "[ERROR]: No file \"" + filePath + "\" found." << std::endl;
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
        }
    }
}

bool validateFileSignature(unsigned short fileSignature[3]) {
    // first three bytes should be 72, 65 and 74 (HAJ in ASCII) when decoded to uints
    if (fileSignature[0] == 72 && fileSignature[1] == 65 && fileSignature[2] == 74) {
        return true;
    }
    return false;
}

bool compileCode(std::string &code) {

}