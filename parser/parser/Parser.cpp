#include "Parser.h"

int Parser::interpret(size_t len, std::string* code) {
    for (int i = 0; i < len; i++) {
        std::cout << *(code + i) << std::endl;
    }

    return 0;
}

