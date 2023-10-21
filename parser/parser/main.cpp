#include "InputStream.h"

int main()
{
    InputStream inputStream;
    std::string filePath = inputStream.getFileLocation();
    std::string* code = inputStream.readFile(filePath);

    for (int i = 0; i < code->size(); i++) {
        std::cout << *(code + i) << std::endl;
    }
}



bool compileCode(std::string &code) {
    return false;
}