using namespace std;

#include <iostream>
#include <fstream>
#include <string>
#include <list>

int main()
{
    // Read from file
    cout << "Please input a filename: ";
    string filename;
    cin >> filename;


    ifstream inputFile(filename);
    if (!inputFile.is_open()) { // If not found return error
        cout << "[ERROR]: No file with name \"" + filename + "\"found.";
        return 1;
    }
    else { // else turn input file to list
        list<string> inputAsList;
        string line;

        while (getline(inputFile, line)) {
            cout << line << "\n";
            inputAsList.push_back(line);
        }

        // then convert to array
        int arrayLength = inputAsList.size();
        string input[arrayLength];


        inputFile.close();
    }


}