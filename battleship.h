// Includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <random.h>

// Functions
/**
 * Reserves memory space for a board of the given dimensions, initializes it with the ships
 * and returns it.
 * @param dim dimensions of the board
 * @return pointer to the initialized board with dimensions dim x dim
 */
char* iniciaTaulellDefensaAutomatic(unsigned int dim);

/**
 * Reserves memory space for a board of the given dimensions and asks the user to place his ships
 * while there are ships remaining. The ships must be placed correctly or the program will show an error
 * message to the user to try again.
 * @param dim
 * @return
 */
char* iniciaTaulellDefensaManualment(unsigned int dim);


/**
 * Reserves memory space for a board of the given dimensions, initializes it with '?'
 * and returns it.
 * @param dim
 * @return
 */
char* iniciaTaulellAtac(unsigned int dim);


/**
 * Locates the limit of the ship pointed by the coordinates x and y and then checks
 * if all the positions of the ship are shot, then return true; otherwise false
 * @param board
 * @param dim
 * @param x
 * @param y
 * @return
 */
bool isSunk(char* board, unsigned int dim, unsigned int x, unsigned int y);


/**
 * Fills with water all the positions sorrounding the ship located by the coordinates x and y
 * @param board
 * @param dim
 * @param x
 * @param y
 */
void floodSorroundings(char* board, unsigned int dim, unsigned int x, unsigned int y);


/**
 * Translates a letter representing a column of the board to the corresponding
 * integer to index in the array. For example 'A' --> 0, 'B' --> 1, ..., 'X' --> dim - 1
 * @param column
 * @return
 */
unsigned int translateColumn(char column);


/**
 * Shows the board given by parameter.
 * @param board
 * @param dim
 */
void showBoard(char* board, unsigned int dim);


/* Aquesta funció actualitza el taulell de vaixells i informa del resultat del llançament:
Casella repetida (0). Quan ja s'ha disparat abans sobre la casella.
Aigua (1)
Tocat (2)
Tocat i enfonsat (3) */

/**
 * This functions updates the ship board and informs the result of the shot:
 *  - Repeated cell (0)
 *  - Water (1)
 *  - Shot (2)
 *  - Shot & Sunk (3)
 * @param board
 * @param dim
 * @param x
 * @param y
 * @return
 */
int shoot(char* board, unsigned int dim, unsigned int x, unsigned int y);


/**
 * Returns two coordinates representing a cell that has been targeted as the next shot.
 * This function
 * @param board
 * @param dim
 */
void computeNextMovement(char* board, unsigned int dim, *unsigned int x, *unsigned int y);
// calcula puntuacio
int calculateScore(dim, *unsigned int shootResults, unsigned int totalShoots)

