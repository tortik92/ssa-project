using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <list>

#define MAX_ARRAY_LENGTH 14

int main()
{
    // Read filename and create a stream
    cout << "Please input a filename: ";
    string filename;
    cin >> filename;
    ifstream inputFile(filename);

    try {
        if (!inputFile.is_open()) {
            throw 1; // File not found
        }
        else {
            // read data from file and store it into array
            string input[MAX_ARRAY_LENGTH];

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
                cout << input[i] << endl;
            }

            
        }
    }
    // --------------------------------------------------------
    // -------------------- ERROR HANDLING --------------------
    // --------------------------------------------------------
    catch (int err) {
        switch (err) {
        case 1: // File not found
            cout << "[ERROR]: No file with name \"" + filename + "\"found." << endl;
            break;
        case 2: // File too long
            cout << "[ERROR]: Maximum amount of lines for input file \"" + filename + "\" exceeded." << endl;
            break;
        }
    }
}

