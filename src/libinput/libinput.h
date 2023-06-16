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
