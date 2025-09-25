#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

// Size of the buffer when reading input
#define READINT_BUFSIZE 64  // plenty for any int (+ spaces, sign, newline)
#define READSTR_BUFSIZE 256  // plenty for typical input

/**
 * Pauses the code execution until any key is pressed.
 */
void pauseExecution();

/**
 * Flushes the stdin buffer .
 */
void discard_rest_of_line(void);

/**
 * Parses an integer from user input and stores it in the supplied buffer.
 *
 * @param s Buffer with the number representation in characters
 * @param out Buffer to store the parsed number as integer
 * @return True if it can parse the integer, false otherwise
 */
bool parse_int_strict(const char *s, int *out);

/**
 * Reads an integer from the user input. If data introduced by the user is not an integer, tries again.
 *
 * @return Read integer.
 */
int readInt();

/**
 * Reads an integer from user input in a given range. While the data introduced is not a integer or is not in range,
 * tries again. The range goes from the minimum number to the maximum number including both of them.
 *
 * @param minimumNumber Minimum integer that is valid, including itself.
 * @param maximumNumber Maximum integer that is valid, including itself.
 * @return Read integer.
 */
int readIntInRange(int minimumNumber, int maximumNumber);

/**
 * Read an integer from user input that is in the supplied set. While the data introduced is not a integer or is not in
 * range, tries again.
 *
 * @param integerSet Set of valid integers that are accepted.
 * @param numIntegerSet Number of integers that we accept as valid. Length of the set.
 * @return Read integer.
 */
int readIntInSet(int* integerSet, int numIntegerSet);

/**
 * Returns true if the supplied integer is in the set, false otherwise.
 *
 * @param integer Integer that we are querying.
 * @param integerSet Set of integers that we accept as valid.
 * @param numIntegerSet Number of integers that we accept as valid. Length of the set.
 * @return True if the integer is in the set, false otherwise.
 */
bool isIntInSet(int integer, int* integerSet, int numIntegerSet);

/**
 * Returns true if the supplied integer is in the range, false otherwise.
 *
 * @param integer Integer that we are querying.
 * @param minimumInt Minimum integer that is valid, including itself.
 * @param maximumInt Maximum integer that is valid, including itself.
 * @return True if the integer is in the range, false otherwise.
 */
bool isIntInRange(int integer, int minimumInt, int maximumInt);

/**
 * Reads a char from user.
 *
 * @return Read char.
 */
char readChar();

/**
 * Reads a char from user input in a given range. While the data introduced is not a char or is not in range, tries
 * again. The range goes from the minimum char to the maximum char including both of them. The range uses the ASCII
 * code.
 *
 * @param minimumChar Minimum char that is valid, including itself.
 * @param maximumChar Maximum integer that is valid, including itself.
 * @return Read char.
 */
char readCharInRange(char minimumChar, char maximumChar);

/**
 * Reads a char from user input in a given set. While the data introduced is not a char or is not in the set, tries
 * again. The set goes from the minimum char to the maximum char including both of them. The range uses the ASCII
 * code.
 *
 * @param characterSet Set of characters that we accept as valid.
 * @param numCharacterSet Number of characters in the accepted character set.
 * @return Read char.
 */
char readCharInSet(char* characterSet, int numCharacterSet);

/**
 * Returns true if the supplied char is in the range, false otherwise. The range is given by the ASCII code.
 *
 * @param letter Character that we are querying.
 * @param minimumChar Minimum character that is valid, including itself.
 * @param maximumChar Maximum character that is valid, including itself.
 * @return True if the char is in the range, false otherwise.
 */
bool isCharInRange(char letter, char minimumChar, char maximumChar);

/**
 * Returns true if the supplied char is in the set, false otherwise.
 *
 * @param letter Character that we are querying.
 * @param characterSet Set of characters that we accept as valid.
 * @param numCharacterSet Number of characters in the set that we accept as valid.
 * @return True if the char is in the set, false otherwise.
 */
bool isCharInSet(char letter, char* characterSet, int numCharacterSet);

/**
 * Reads a line from stdin, discards newline
 * and repeats until a non-empty string is read.
 * Always returns an allocated string (never NULL).
 * @return Pointer to a buffer containing the read string
 */
char *readString(void);

/**
 * Copies n bytes from source to destination.
 *
 * @param dest Pointer to the destination the copy.
 * @param src Pointer to the source of the data to copy.
 * @param n Number of bytes to copy.
 * @return Destination address.
 */
void* memcpy(void* dest, const void* src, size_t n);

/**
 * Flush the stdin so further calls to input reading functions such as scanf or fgets don't read ghost characters
 */
void flushStdin();