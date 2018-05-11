#pragma once

#define STATUS_OK 0
#define ERR_INV_ARG -1
#define ERR_NOT_FOUND -2
#define ERR_SIZE -3

#include "string.h"

/**
 * Converts all characters in inStr to lower case.
 *
 * @param inStr C string
 */
void strToLower(char *inStr);

/**
 * Converts all characters in inStr to upper case.
 *
 * @param inStr C string
 */
void strToUpper(char *inStr);

/**
 * Searches for character in string.
 *
 * @param inStr C string
 * @param findChar character to search for
 *
 * @return pointer to the first occurence of character, or NULL
 * if the character was not found.
 */
char *strFindChar(char *inStr, char findChar);

/**
 * Copies substring from input string to output string.
 *
 * @param inStr input C string
 * @param offset index of the first character in input string
 * @param len the number of characters to copy
 * @param outStr output C string
 *               Should also contain space for terminating '\0'.
 * @param outStrSize number of bytes allocated in output string.
 *
 * @return STATUS_OK if successful, ERR_INV_ARG if offset and len
 *         indicate chars out of input string, ERR_SIZE if 
 *         there is not enough space in output string.
 */
int strSubstr(char *inStr, size_t offset, size_t len, char *outStr, size_t outStrSize);

/**
 * Copies substring from input string to output string.
 *
 * @param inStr input C string
 * @param startChar character which starts substring
 * @param endChar character after startChar, which ends substring
 * @param outStr output C string
 *               Should also contain space for terminating '\0'.
 * @param outStrSize number of bytes allocated in output string
 *
 * @return STATUS_OK if successful, ERR_NOT_FOUND start char was not found,
 *         ERR_SIZE if there is not enough space in output string.
 */
int strSubstrChar(char *inStr, char startChar, char endChar, char *outStr, size_t outStrSize);

/**
 * Replace all charcter in input string, which match oldCahr with newChar.
 *
 * @param inStr input C string
 * @param oldChar character to replace
 * @param newChar replacement character
 */
int strReplaceChar(char *inStr, char oldChar, char newChar);
