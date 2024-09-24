#pragma once

#include <string.h>

#include "Constants.h"

class StringHelper {
public:
  /**
	 * Checks if the given string is a nullptr or "".
	 *
	 * \param string String to check
	 * \return true if empty, false if not
	 */
  static bool isEmpty(const char* string) {
    return string == nullptr || strcmp(string, "") == 0;
  }

  /**
	 * Finds the needle in the haystack.
	 *
	 * \param haystack The string to be searched
	 * \param needle The string to search for
	 * \return true if the needle is found in the haystack
	 */
  static bool contains(const char* haystack, const char* needle) {
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
	 * \param originalLen Length of the original string
	 * \param toReplace The substring that should be replaced
	 * \param replacement The string that is used as a replacement
   * \return success if successful, valueTooLarge if original is too small for replacement
	 */
  static int replace(char* original, size_t originalLen, const char* toReplace, const char* replacement) {
    size_t toReplaceLen = strlen(toReplace);
    size_t replacementLen = strlen(replacement);

    size_t i = 0;
    while (i < originalLen) {
      if (strncmp(&original[i], toReplace, toReplaceLen) == 0) {
        if (toReplaceLen == replacementLen || toReplaceLen > replacementLen) {
          strcpy(&original[i], replacement);

          if (toReplaceLen > replacementLen) {
            int excessSpace = toReplaceLen - replacementLen;

            for (size_t j = i + replacementLen; j < originalLen - excessSpace; j++) {
              original[j] = original[j + excessSpace];
            }

            original[originalLen - excessSpace] = '\0';
          }

          i += replacementLen;
        } else {  // replacement is bigger than toReplace
          int availableSpace = originalLen - strlen(original);
          int requiredExtraSpace = replacementLen - toReplaceLen;

          if (availableSpace < requiredExtraSpace) return valueTooLarge;
          else {
            for (size_t j = originalLen - 1; j >= i + toReplaceLen; j--) {
              if (j + requiredExtraSpace < originalLen) {
                original[j + requiredExtraSpace] = original[j];
              }
            }
          }
        }
      } else {
        i++;
      }
    }

    return success;
  }

  /**
	 * Replaces the substring in a string with the given number.
	 * 
	 * \param original The string where the replacement is performed
	 * \param originalLen Length of the original string
	 * \param toReplace The substring that should be replaced
	 * \param replacement The number that is used as a replacement
	 */
  static void replaceNum(char* original, size_t originalLen, const char* toReplace, int replacement) {
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
	 * \throw emptyInput When original or substr are empty
	 * \throw bufferTooSmall When the insertion would violate the boundaries of original
	 * \return success when successful
	 */
  static void insert(char* original, size_t originalLen, size_t position, const char* substr) {
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
	 * \throw bufferTooSmall if unable to fit the number in the given storageLocation
	 * \return success if successful
	 */
  static void toString(int num, char* storeLoc, size_t storeLocLen) {
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
    while (storeLoc[i + j + 1] != '\0') {
      storeLoc[j] = storeLoc[i + j + 1];
      storeLoc[i + j + 1] = '\0';
      j++;
    }
    storeLoc[j] = '\0';
  }

  /**
   * Securely copies the value of one string to another.
   * 
   * \param dest the destination string
   * \param destSize the size of the destination
   * \param src the source string
   */
  static int strcpy_s(char* dest, size_t destSize, const char* src) {
    if (dest == nullptr || src == nullptr) {
      return invalidValue;
    }

    size_t count = strlen(src);

    return strncpy_s(dest, destSize, src, count);
  }


  /**
   * Securely copies n characters of one string to another.
   *
   * \param dest the destination string
   * \param destSize the size of the destination
   * \param src the source string
   * \param count how many characters should be copied from src to dest 
   */
  static int strncpy_s(char* dest, size_t destSize, const char* src, size_t count) {
    if (dest == nullptr || src == nullptr) {
      return invalidValue;
    }

    size_t srcLen = strlen(src);

    if (srcLen + 1 > destSize || destSize < count) {
      dest[0] = '\0';
      return valueOutOfRange;
    }

    for (size_t i = 0; i < srcLen; i++) {
      dest[i] = src[i];
    }
    dest[srcLen] = '\0';

    return success;
  }

  /**
   * Concatenates one string to another.
   * 
   * \param dest the destination string
   * \param destSize the size of the destination
   * \param src the source string
   */
  static int strcat_s(char* dest, size_t destSize, const char* src) {
    if (dest == nullptr || src == nullptr || destSize <= 0) {
      return invalidValue;
    }

    size_t destStrLen = strlen(dest);
    size_t srcLen = strlen(src);
    size_t sumLen = destStrLen + srcLen;

    if (sumLen >= destSize) {
      return valueOutOfRange;
    }

    for (size_t i = 0; i < srcLen; i++) {
      dest[destStrLen + 1 + i] = src[i];
    }
    dest[sumLen] = '\0';
    return success;
  }
};
