#pragma once

#include <string.h>

class StringHelper
{
public:
	/**
	 * Checks if the given string is a nullptr or "".
	 *
	 * \param string String to check
	 * \return true if empty, false if not
	 */
	bool isEmpty(const char* string);
	
	/**
	 * Finds the needle in the haystack.
	 *
	 * \param haystack The string to be searched
	 * \param needle The string to search for
	 * \return if the needle is found in the haystack
	 */
	bool contains(const char* haystack, const char* needle);
	
	/**
	 * Replaces a substring in a string with another string.
	 *
	 * \param original The string where the replacement is performed
	 * \param originalLen Length of the original string
	 * \param toReplace The substring that should be replaced
	 * \param replacement The string that is used as a replacement
	 */
	void replace(char* original, size_t originalLen, const char* toReplace, const char* replacement);

	/**
	 * Replaces the substring in a string with the given number.
	 * 
	 * \param original The string where the replacement is performed
	 * \param originalLen Length of the original string
	 * \param toReplace The substring that should be replaced
	 * \param replacement The number that is used as a replacement
	 */
	void replaceNum(char* original, size_t originalLen, const char* toReplace, int replacement);
	
	/**
	 * Inserts a substring at a given position.
	 *
	 * \param original Where to insert to
	 * \param originalLen How long the original is
	 * \param position Where to insert the substring
	 * \param substr The string to be inserted
	 * \throw emptyInput When original or substr are empty
	 * \throw bufferTooSmall When the insertion would violate the boundaries of original
	 * \return success when successful
	 */
	void insert(char* original, size_t originalLen, int position, const char* substr);
	
	/**
	 * Converts a number to a string.
	 *
	 * \param num The number to be converted
	 * \param storageLocation Where the converted number is stored
	 * \param storageLocationLen How long storageLocation is
	 * \throw bufferTooSmall if unable to fit the number in the given storageLocation
	 * \return success if successful
	 */
	void toString(int num, char* storageLocation, size_t storageLocationLen);
};

