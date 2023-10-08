#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::string* inputstring = readfile();
}

std::string* readfile() {
    // Read filename
    std::cout << "Please input a filename: ";
    std::string filename;
    std::cin >> filename;
    std::ifstream inputfile(filename); // create stream

    try {
        if (!inputfile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read first line and check for data
            std::string firstline;
            getline(inputfile, firstline);

            // First three bytes should be 72, 65 and 74 (HAJ) when decoded to uints
            unsigned int filesignature[3] = { 
                std::stoi(firstline.substr(0, 2), 0, 16), 
                std::stoi(firstline.substr(2, 2), 0, 16), 
                std::stoi(firstline.substr(4, 2), 0, 16) 
            };

            if (filesignature[0] == 72 && filesignature[1] == 65 && filesignature[2] == 74) {
                // read data from file and store it into array
                int nroflines = 0;

                std::string* inputarray = new std::string[nroflines];

                int arrayindex = 0;
                while (!inputfile.eof()) {
                    getline(inputfile, inputarray[nroflines]);
                }

                inputfile.close(); // clean up
                return inputarray;
            } 
            else {
                throw 3;
            }
            
        }
    }
    catch (int err) {
        switch (err) {
        case 1: // File not found
            std::cout << "[ERROR]: No file \"" + filename + "\" found." << std::endl;
            inputfile.close();
            break;
        case 2: // File too long
            std::cout << "[ERROR]: The file \"" + filename + "\" is longer than stated in the first parameter of the file.";
            inputfile.close();
            break;
        case 3: // Invalid File signature
            std::cout << "[ERROR]: The file \"" + filename + "\" does not have the correct signature.";
            inputfile.close();
            break;
        }
    }
}