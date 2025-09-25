#include "libinput.h"

void pauseExecution()
{
    char a;
    if (scanf("%c", &a))
        return;
}

void discard_rest_of_line(void) {
    int c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF) {}
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
    char buf[READINT_BUFSIZE];

    for (;;) {
        if (fgets(buf, READINT_BUFSIZE, stdin) == NULL) {
            continue;  // If EOF or error try again
        }

        // Handle long lines: if newline didn't fit, discard the rest
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n') {
            discard_rest_of_line();
        } else if (len > 0) {
            // strip newline
            buf[len - 1] = '\0';
        }

        int value;
        if (parse_int_strict(buf, &value)) {
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
    char buf[READSTR_BUFSIZE];

    for (;;) {
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            // EOF or error — just keep waiting for valid input
            clearerr(stdin);
            continue;
        }

        // If no newline, input was too long — discard rest of the line
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] != '\n') {
            discard_rest_of_line();
        } else if (len > 0) {
            buf[len - 1] = '\0'; // strip newline
        }

        // Allocate and return
        char *result = malloc(strlen(buf) + 1);
        if (!result) {
            // If allocation fails, prompt again
            continue;
        }
        strcpy(result, buf);
        return result;
    }
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