#include "Expression.h"


int Expression::parseExpression(char* input) {
	// remove all whitespaces in expression
	for (int i = 0; i < strlen(input); i++) {
		if (input[i] == ' ') {
			// shift every character one to the left
			for (int j = i; j < strlen(input); j++) {
				input[j - 1] = input[j];
			}
		}
	}

	// insert a '+' before each '-' to make the calculations easier
	int offset = 0;
	while (true) {
		size_t minusPos = input.find('-', offset);

		if (minusPos != NULL) {
			if (minusPos > 0 && input[minusPos - 1] != '+') {
				input.insert(minusPos, 1, '+');
				offset = minusPos + 2;
			}
		}
		else break;
	}

	return expression(input);
}

int Expression::expression(char* input, size_t offset) {
	size_t plusLoc = input.find('+', offset);
	size_t openParentLoc = input.find('(', offset);
	size_t closeParentLoc = input.rfind(')'); // no offset for rfind, as it searches backwards (so beginning from offset 0 it will try to search at index -1 and return npos)

	if (plusLoc != NULL) {
		if ((plusLoc < openParentLoc || plusLoc > closeParentLoc)) { // if '+' is before '(' or after ')'
			return 
				term(input.substr(0, plusLoc));
				+ expression(input.substr(plusLoc + 1, input.length() - plusLoc - 1));
		}
		else {
			// find operators after ')'
			if (closeParentLoc != NULL) { 
				return expression(input, closeParentLoc);
			}
			else { // else there is no '+' outside of parentheses
				return term(input);
			}
		}
	}
	else { // if there is no '+'
		return term(input, offset);
	}

}

int Expression::term(char* input, size_t offset) {
	size_t multiplyLoc = input.find('*', offset);

	if (multiplyLoc != NULL) {
		return 
			factor(input.substr(0, multiplyLoc)) 
			* term(input.substr(multiplyLoc + 1, input.length() - multiplyLoc - 1));
	}
	else {
		size_t divideLoc = input.find('/', offset);

		if (divideLoc != NULL) {
			return 
				factor(input.substr(0, divideLoc))
				/ term(input.substr(divideLoc + 1, input.length() - divideLoc - 1));
		}
		else {
			return factor(input);
		}
	}
}

int Expression::factor(char* input, size_t offset) {
	size_t openParentLoc = input.find('(', offset);

	if (openParentLoc != NULL) {
		size_t closeParentLoc = input.rfind(')');

		if (closeParentLoc == NULL) {
			throw std::invalid_argument("Expected ')'");
		}
		else {
			return expression(input.substr(openParentLoc + 1, closeParentLoc - openParentLoc - 1));
		}
	}
	else {
		return parseNumber(input);
	}
}

