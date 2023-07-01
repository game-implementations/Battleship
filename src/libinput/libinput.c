#include "libinput.h"

void pauseExecution()
{
    char a;
    if (scanf("%c", &a))
        return;
}

int readInt(unsigned int maximum_characters_accepted)
{
    char* endptr, *readInput = calloc(maximum_characters_accepted, sizeof(char));
    int number = 0;

    do
    {
        if (fgets(readInput, maximum_characters_accepted, stdin) == NULL)
        {
            continue;
        }

        number = strtol(readInput, &endptr, 10);
    }
    while (*endptr != '\n');

    free(readInput);
    return number;
}

int readIntInRange(int minimumNumber, int maximumNumber, unsigned int maximum_characters_accepted)
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
        integerInRange = readInt(maximum_characters_accepted);
        printf("\n");
        hasRangeError = true;
    }
    while(integerInRange > maximumNumber || integerInRange < minimumNumber);
    return integerInRange;
}


void flushStdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}


char readChar()
{
    char* readInput = calloc(100, sizeof(char));
    do
    {
        if (fgets(readInput, 100, stdin) == NULL)
        {
            continue;
        }
    }
    while (readInput[1] != '\n');

    return readInput[0];
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

int readIntInSet(int* integerSet, int numIntegerSet, unsigned int maximum_characters_accepted)
{
    int readInteger;
    bool hasSetError = false;
    do
    {
        if (hasSetError)
        {
            printf("You have given a int out of range. Try again.\n");
        }
        readInteger = readInt(maximum_characters_accepted);
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

void* memcpy(void* dest, const void* src, size_t n)
{
    // Initialize pointers with implicit size of a byte
    char* src_byte = (char*) src;
    char* dest_byte = (char*) dest;

    for (unsigned int i = 0; i < n; i++)
    {
        dest_byte[i] = src_byte[i];
    }
    return dest;
}