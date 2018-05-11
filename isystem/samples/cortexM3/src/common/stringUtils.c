#include "stringUtils.h"

#include "ctype.h"
#include "string.h"

void strToLower(char *inStr)
{
    while (*inStr != '\0') {
        *inStr = tolower(*inStr);
        inStr++;
    }
}


void strToUpper(char *inStr)
{
    while (*inStr != '\0') {
        *inStr = toupper(*inStr);
        inStr++;
    }
}


char *strFindChar(char *inStr, char findChar)
{
    while (*inStr != '\0') {
        if (*inStr == findChar) {
            return inStr;
        }
        inStr++;
    }

    return NULL;
}


int strSubstr(char *inStr, size_t offset, size_t len, char *outStr, size_t outStrSize)
{
    if (strlen(inStr) < offset + len) {
        return ERR_INV_ARG;
    }

    // make space for terminating null char
    if (outStrSize < len + 1) {
        return ERR_SIZE;
    }

    inStr += offset;
    for (size_t idx = offset; idx < offset + len; idx++, outStr++, inStr++) {
        *outStr = *inStr;
    }
    
    *outStr = '\0';

    return STATUS_OK;
}


int strSubstrChar(char *inStr, char startChar, char endChar, char *outStr, size_t outStrSize)
{
    char *startPtr = strFindChar(inStr, startChar);

    if (startPtr == NULL) {
        return ERR_NOT_FOUND;
    }

    char *destPtr = outStr;
    while (*startPtr != '\0'  &&  *startPtr != endChar) {
        *destPtr++ = *startPtr++;

        size_t currentSize = destPtr - outStr;
        if (currentSize >= outStrSize) {
            outStr[outStrSize - 1] = '\0';
            return ERR_SIZE;
        }
    }

    *destPtr = '\0';
    
    return STATUS_OK;
}


int strReplaceChar(char *inStr, char oldChar, char newChar)
{
    int count = 0;
    
    while (*inStr != '\0') {
        if (*inStr == oldChar) {
            *inStr = newChar;
            count++;
        }
        inStr++;
    }

    return count;
}
