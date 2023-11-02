#include "Parser.h"

int Parser::interpret(size_t len, std::string* code) {
    for (int i = 0; i < len; i++) {
        std::cout << *(code + i) << "\n";
    }
    std::cout << "Successfully iterated through the array!";
    return 0;
}

