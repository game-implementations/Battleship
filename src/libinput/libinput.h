#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Maximum number of characters in the user input to be read
#define MAX_CHAR_USER_INPUT 1000

/**
 * Pauses the code execution until any key is pressed.
 */
void pauseExecution();

/**
 * Tries to read an int from the user input until it achieves it.
 * 
 * @param maximum_characters_accepted
 * @return
 */
int readInt(unsigned int maximum_characters_accepted);

/**
 * Tries to read an integer in a given range from user input until the range is satisfied.
 * The range goes from the minimum number to the maximum number including both of them.
*/
int readIntInRange(int minimumNumber, int maximumRange, unsigned int maximum_characters_accepted);

/**
 *
*/
int readIntInSet(int* integerSet, int numIntegerSet, unsigned int maximum_characters_accepted);

/**
 *
*/
bool isIntInSet(int integer, int* integerSet, int numIntegerSet);

/**
 *
*/
bool isIntInRange(int index, int minimumInt, int maximumInt);

/**
 * Tries to read a char from the user input until it achieves it.
*/
char readChar();

/**
 * Tries to read a char in the range from minimumChar to maximumChar in ASCII code.
*/
char readCharInRange(char minimumChar, char maximumChar);

/**
 *
*/
char readCharInSet(char* characterSet, int numCharacterSet);

/**
 * Flush the stdin so further calls to input reading functions such as scanf or fgets don't read ghost characters
*/
void flushStdin();


/**
 *
*/
bool isCharInRange(char letter, char minimumChar, char maximumChar);

/**
 *
*/
bool isCharInSet(char letter, char* characterSet, int numCharacterSet);

void* memcpy(void* dest, const void* src, size_t n)
