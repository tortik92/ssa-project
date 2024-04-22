#include "StringHelper.h"

/**
 * Checks if the given string is a nullptr or "".
 * 
 * \param string String to check
 * \return true if empty, false if not
 */
bool StringHelper::isEmpty(const char* string) {
    return string == nullptr || string == "";
}

/**
 * Finds the needle in the haystack.
 *
 * \param haystack The string to be searched
 * \param needle The string to search for
 * \return if the needle is found in the haystack
 */
bool StringHelper::contains(const char* haystack, const char* needle)
{
    size_t haystackLen = strlen(haystack);
    size_t needleLen = strlen(needle);

    for (size_t i = 0; i < haystackLen; i++) {
        if (haystack[i] == needle[0]) {
            size_t j = 0;
            for (j = 0; j < needleLen && i + j < haystackLen; j++) {
                if (haystack[i + j] != needle[j]) {
                    break;
                }
            }

            if (j == needleLen) {
                return true;
            }
        }
    }

    return false;
}

/**
 * Replaces a substring in a string with another string.
 * 
 * \param original The string where the replacement is performed
 * \param toReplace The substring that should be replaced
 * \param replacement The string that is used as a replacement
 */
void StringHelper::replace(char* original, size_t originalLen, const char* toReplace, const char* replacement)
{
    size_t toReplaceLen = strlen(toReplace);
    size_t replacementLen = strlen(replacement);

    size_t i = 0;
    while (i < originalLen)
    {
        if (strncmp(&original[i], toReplace, toReplaceLen) == 0) {
            strncpy_s(&original[i], originalLen, replacement, replacementLen);
            i += replacementLen;
        }
        else {
            i++;
        }
    }
}

void StringHelper::replaceNum(char* original, size_t originalLen, const char* toReplace, int replacement) {
    const short shortAsStrLen = 5;
    char numAsStr[shortAsStrLen] = "";
    toString(replacement, numAsStr, shortAsStrLen);
    replace(original, originalLen, toReplace, numAsStr);
}

/**
 * Inserts a substring at a given position.
 * 
 * \param original Where to insert to
 * \param originalLen How long the original is
 * \param position Where to insert the substring
 * \param substr The string to be inserted
 */
void StringHelper::insert(char* original, size_t originalLen, int position, const char* substr)
{
    size_t substrLen = strlen(substr);

    size_t i;
    for (i = originalLen; i >= position; i--) {
        original[i + substrLen] = original[i];
    }
    for (i = 0; i < substrLen; i++) {
        original[position + i] = substr[i];
    }
}

/**
 * Converts a number to a string.
 * 
 * \param num The number to be converted
 * \param storageLocation Where the converted number is stored
 * \param storageLocationLen How long storageLocation is
 */
void StringHelper::toString(int num, char* storeLoc, size_t storeLocLen) {
    bool isNegative = false;
    
    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    size_t i = storeLocLen;
    while (num != 0) {
        storeLoc[i--] = (num % 10) + '0';
        num /= 10;
    }

    if (isNegative) {
        storeLoc[i--] = '-';
    }

    size_t j = 0;
    while(storeLoc[i + j + 1] != '\0') {
        storeLoc[j] = storeLoc[i + j + 1];
        storeLoc[i + j + 1] = '\0';
        j++;
    }
    storeLoc[j] = '\0';
}