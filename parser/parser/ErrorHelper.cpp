#include "ErrorHelper.h"

void ErrorHelper::throwError(const char errorMessage[]) {
	std::cerr << "[ERROR]: " << errorMessage << "\n";
}