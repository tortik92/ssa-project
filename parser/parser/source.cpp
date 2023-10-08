#include <iostream>
#include <fstream>
#include <string>

int main()
{
    // Read filename
    std::cout << "Please input a filename: ";
    std::string filename;
    std::cin >> filename;

    std::string* inputArray = readFile(filename);
}

std::string* readFile(std::string filename) {
    std::ifstream inputFile(filename); // create stream

    try {
        if (!inputFile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read data from file and store it into array
            int nrOfLines = 0;


            std::string* inputArray = new std::string[nrOfLines];

            int arrayIndex = 0;
            while (!inputFile.eof()) {
                getline(inputFile, inputArray[nrOfLines]);
            }

            inputFile.close(); // clean up
            return inputArray;
        }
    }
    catch (int err) {
        switch (err) {
        case 1: // File not found
            std::cout << "[ERROR]: No file \"" + filename + "\" found." << std::endl;
            inputFile.close();
            break;
        case 2: // File too long
            std::cout << "[ERROR]: The file \"" + filename + "\" is longer than stated in the first parameter of the file.";
            inputFile.close();
        }
    }
}