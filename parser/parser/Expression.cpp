#include "Expression.h"


float Expression::parseExpression(std::string input) {
	input.erase(remove(input.begin(), input.end(), ' '), input.end()); // remove all whitespaces in expression
	return expression(input);
}

float Expression::expression(std::string input, size_t offset) {
	size_t plusLoc = input.find('+', offset);
	size_t minusLoc = input.find('-', offset);
	size_t openParentLoc = input.find('(', offset);
	size_t closeParentLoc = input.rfind(')'); // no offset for rfind, as it searches backwards (so beginning from offset 0 it will try to search at index -1 and return npos)

	if (plusLoc != std::string::npos && plusLoc < minusLoc) {
		if ((plusLoc < openParentLoc || plusLoc > closeParentLoc)) { // if '+' is before '(' or after ')' and '+' is before '-'
			float t = term(input.substr(0, plusLoc));
			float e = expression(input.substr(plusLoc + 1, input.length() - plusLoc - 1));

			std::cout << "Expression: " << t << " + " << e << "\n";

			return t + e;
		}
		else {
			if (closeParentLoc != std::string::npos) { // find operators after ')'
				return expression(input, closeParentLoc);
			}
			else { // else there is no '+' outside of parentheses
				return term(input);
			}
		}
	}
	if (minusLoc != std::string::npos) { // if there is no '+'
		if (minusLoc < openParentLoc || minusLoc > closeParentLoc) { // if '-' is before '(' or after ')'
			float t = term(input.substr(0, minusLoc));
			float e = expression(input.substr(minusLoc + 1, input.length() - minusLoc - 1));

			std::cout << "Expression: " << t << " - " << e << "\n";

			return t - e;
		}
		else {
			if (closeParentLoc != std::string::npos) { // find operators after ')'
				return expression(input, closeParentLoc);
			}
			else { // else there is no '-' outside of parentheses
				return term(input);
			}
		}
	}
	else { // if there is no '+' or '-'
		return term(input, offset);
	}

}

float Expression::term(std::string input, size_t offset) {
	size_t multiplyLoc = input.find('*', offset);

	if (multiplyLoc != std::string::npos) {
		float f = factor(input.substr(0, multiplyLoc));
		float t = term(input.substr(multiplyLoc + 1, input.length() - multiplyLoc - 1));

		std::cout << "Term: " << f << " * " << t << "\n";

		return f * t;
	}
	else {
		size_t divideLoc = input.find('/', offset);

		if (divideLoc != std::string::npos) {
			float f = factor(input.substr(0, divideLoc));
			float t = term(input.substr(divideLoc + 1, input.length() - divideLoc - 1));

			std::cout << "Term: " << f << " / " << t << "\n";

			return f / t;

		}
		else {
			return factor(input);
		}
	}
}

float Expression::factor(std::string input, size_t offset) {
	size_t openParentLoc = input.find('(', offset);

	if (openParentLoc != std::string::npos) {
		size_t closeParentLoc = input.rfind(')');

		if (closeParentLoc == std::string::npos) {
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

float Expression::parseNumber(std::string numberAsString, bool requiredPositive) {
	float parsedNumber = 0;

	// remove leading and trailing spaces/unwanted characters
	const char notLikedInNumbers[] = " \t\n\r\f\v";
	numberAsString.erase(0, numberAsString.find_first_not_of(notLikedInNumbers));
	numberAsString.erase(numberAsString.find_last_not_of(notLikedInNumbers) + 1);

	// return macro
	if (numberAsString == "ACTIVE_COUNT") {
		return padsCount;
	}
	// return random number
	size_t randomPos = numberAsString.find("random(");
	if (randomPos != std::string::npos) {
		size_t closeParentPos = numberAsString.rfind(")");
		if (closeParentPos != std::string::npos) {
			// extract number from random method
			std::string numberInRandomPar = numberAsString.substr(randomPos + 7, closeParentPos - randomPos - 7);

			// make random number
			parsedNumber = (float)(std::rand() % (int)parseNumber(numberInRandomPar, true));
		}
		else {
			throw std::invalid_argument("Missing ')'");
		}
	}
	else {
		try {
			parsedNumber = std::stof(numberAsString);
		}
		catch (const std::invalid_argument) {
			throw std::invalid_argument("Expression must be a number");
		}

		if (requiredPositive && parsedNumber < 0) {
			throw std::invalid_argument("Expression must be a positive number");
		}
	}

	return parsedNumber;
}

float Expression::parseNumber(std::string number, bool requiredPositive, int maxValue) {
	float parsedNumber = parseNumber(number, requiredPositive);

	if (parsedNumber > maxValue) {
		throw std::invalid_argument(std::string("Expression must be a number smaller than " + maxValue).c_str());
	}

	return parsedNumber;
}