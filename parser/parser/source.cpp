#include <iostream>
#include <fstream>
#include <string>

#define MAX_ARRAY_LENGTH 14

int main()
{
    // Read filename and create a stream
    std::string filename;

    std::cout << "Please input a filename: ";
    std::cin >> filename;

    std::ifstream inputFile(filename);

    try {
        if (!inputFile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read data from file and store it into array
            std::string input[MAX_ARRAY_LENGTH];

            int nrOfLines = 0;
            while (!inputFile.eof()) {
                getline(inputFile, input[nrOfLines]);
                nrOfLines++;

                if (nrOfLines > MAX_ARRAY_LENGTH) {             
                    throw 2; // file too long
                }
            }
            inputFile.close(); // clean up

            for (int i = 0; i < nrOfLines; i++) {
                std::cout << input[i] << std::endl;
            }

            
        }
    }
    catch (int err) {
        switch (err) {
        case 1: // File not found
            std::cout << "[ERROR]: No file with name \"" + filename + "\"found." << std::endl;
            break;
        case 2: // File too long
            std::cout << "[ERROR]: Maximum amount of lines for input file \"" + filename + "\" exceeded." << std::endl;
            break;
        }
    }
}

