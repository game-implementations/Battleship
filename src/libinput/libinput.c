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

int readIntInRange(int minimumNumber, int maximumRange, unsigned int maximum_characters_accepted)
{
    int integerInRange;
    bool hasRangeError = false;
    do
    {
        if (hasRangeError)
        {
            printf("You have given an integer out of range.\n");
        }
        printf("Introduce an integer from %i to %i:\t", minimumNumber, maximumRange);
        integerInRange = readInt(maximum_characters_accepted);
        printf("\n");
        hasRangeError = true;
    }
    while(integerInRange > maximumRange || integerInRange < minimumNumber);
    return integerInRange;
}





