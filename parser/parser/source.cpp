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

    if (!inputFile.is_open()) { 
        cout << "[ERROR]: No file with name \"" + filename + "\"found." << endl; // File not found error
        return 1;
    }
    else { 
        // read data from file and store it into array
        string input[MAX_ARRAY_LENGTH];
        
        int nrOfLines = 0;
        while (!inputFile.eof()) {
            getline(inputFile, input[nrOfLines]);
            nrOfLines++;

            if (nrOfLines > MAX_ARRAY_LENGTH) {
                cout << "[ERROR]: Maximum amount of lines for input file \"" + filename + "\" exceeded." << endl; // file too long error
                break;
            }
        }

        for (int i = 0; i < nrOfLines; i++) {
            cout << input[i] << endl;
        }

        // clean up
        inputFile.close();
    }
}

