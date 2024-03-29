#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Pauses the code
 */
void pauseExecution();

/**
 * Tries to read an int from the user input until it achieves it.
*/
int readInt(unsigned int maximum_characters_accepted);

/**
 * Tries to read an integer in a given range from user input until the range is satisfied.
 * The range goes from the minimum number to the maximum number including both of them.
*/
int readIntInRange(int minimumNumber, int maximumRange, unsigned int maximum_characters_accepted);

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
bool isIntInRange(int index, int minimumInt, int maximumInt);

/**
 *
*/
bool isCharInRange(char letter, char minimumChar, char maximumChar);

/**
 *
*/
bool isCharInSet(char letter, char* characterSet, int numCharacterSet);
