#include "libinput.h"
#include "../libplatform/platform.h"

void pauseExecution()
{
    platform_wait_any_key();
}

bool parse_int_strict(const char *s, int *out) {
    if (!s || !out) return false;

    // Skip leading whitespace
    while (isspace((unsigned char)*s)) s++;

    if (*s == '\0') return false; // empty or all-space

    errno = 0;
    char *endptr = NULL;
    long v = strtol(s, &endptr, 10);

    if (s == endptr) return false;                    // no digits
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX) return false;

    // Skip trailing whitespace
    while (isspace((unsigned char)*endptr)) endptr++;

    if (*endptr != '\0') return false;                // trailing junk

    *out = (int)v;
    return true;
}

int readInt(void) {
    for (;;) {
        char *line = platform_read_line();  // never NULL, newline already stripped
        int value;
        bool ok = parse_int_strict(line, &value);
        free(line);
        if (ok) {
            return value;
        }
    }
}

int readIntInRange(int minimumNumber, int maximumNumber)
{
    int integerInRange;
    bool hasRangeError = false;
    do
    {
        if (hasRangeError)
        {
            printf("You have given an integer out of range.\n");
        }
        printf("Introduce an integer from %i to %i:\t", minimumNumber, maximumNumber);
        integerInRange = readInt();
        printf("\n");
        hasRangeError = true;
    }
    while(integerInRange > maximumNumber || integerInRange < minimumNumber);
    return integerInRange;
}

char readChar()
{
    for (;;)
    {
        char *line = platform_read_line();
        // Accept only a line that holds exactly one character, matching the
        // previous "single char followed by newline" contract.
        if (strlen(line) == 1)
        {
            char c = line[0];
            free(line);
            return c;
        }
        free(line);
    }
}

char readCharInRange(char minimumChar, char maximumChar)
{
    char charInRange;
    bool hasRangeError = false;
    do
    {
        if (hasRangeError)
        {
            printf("You have given a char out of range.\n");
        }
        printf("Introduce a char from %c to %c:\t", minimumChar, maximumChar);
        charInRange = readChar();
        printf("\n");
        hasRangeError = true;
    }
    while(charInRange > maximumChar || charInRange < minimumChar);

    return charInRange;
}

bool isCharInRange(char letter, char minimumChar, char maximumChar)
{
    return !(letter > maximumChar || letter < minimumChar);
}

bool isCharInSet(char letter, char* characterSet, int numCharacterSet)
{
    for (int i = 0; i < numCharacterSet; i++)
    {
        if (characterSet[i] == letter)
        {
            return true;
        }
    }
    return false;
}

char readCharInSet(char* characterSet, int numCharacterSet)
{
    char readCharacter;
    bool hasSetError = false;
    do
    {
        if (hasSetError)
        {
            printf("You have given a char out of range. Try again.\n");
        }
        readCharacter = readChar();
        hasSetError = !isCharInSet(readCharacter, characterSet, numCharacterSet);
    }
    while (hasSetError);
    return readCharacter;
}

bool isIntInRange(int letter, int minimumInt, int maximumInt)
{
    return letter >= minimumInt && letter <= maximumInt;
}

int readIntInSet(int* integerSet, int numIntegerSet)
{
    int readInteger;
    bool hasSetError = false;
    do
    {
        if (hasSetError)
        {
            printf("You have given a int out of range. Try again.\n");
        }
        readInteger = readInt();
        hasSetError = !isIntInSet(readInteger, integerSet, numIntegerSet);
    }
    while(hasSetError);
    return readInteger;
}

bool isIntInSet(int integer, int* integerSet, int numIntegerSet)
{
    for (int i = 0; i < numIntegerSet; i++)
    {
        if (integerSet[i] == integer)
        {
            return true;
        }
    }
    return false;
}

char *readString(void) {
    // The platform layer already returns an owned, newline-stripped buffer and
    // never returns NULL, so this is now just a pass-through.
    return platform_read_line();
}