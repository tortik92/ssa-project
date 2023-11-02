#include "ErrorHelper.h"

void ErrorHelper::reportError(const char errorMessage[]) {
	std::cerr << "[ERROR]: " << errorMessage << "\n";
}