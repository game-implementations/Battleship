// INCLUDES
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "DoubleLinkedList.h"


// DEFINES
#define NOT_DISCOVERED_CELL '?'
#define WATER '~'
#define SHOT_WATER '-'
#define SHIP '@'
#define SHOT_SHIP 'X'


// CONSTANT VALUES
/**
 * Defines the number of ships for each ship of different size.
 * For example, NUM_SHIPS_TYPE[0] gives the number of ships of size 1
 * that we are going to have in the game.
 **/
const unsigned char NUM_SHIPS_BY_SIZE[] = {4, 3, 2, 1};
const unsigned char SHIP_MAX_SIZE = 4;


// TYPE DEFINITION STRUCTURE FOR A PLAYER
typedef struct Player {
    unsigned char* attackBoard;  // Points to the board containing the revealed positions of the enemy
    unsigned char* defenseBoard;  // Points to the board that contains our ships
    DoubleLinkedList* tableResultMoves;  // Contains all the results from the shots of this player
} Player;


// GLOBAL VARIABLE DEFINITIONS
extern unsigned char DIM;  // Contains the dimension of the board. Most of functions use this variable


// PROCEDURE-LIKE (STATIC) FUNCTIONS
/**
 * Reads safely from user input a variable of the type specified by the type parameter.
 * Forces the user to introduce a valid input. Stores data read in readed_variable.
 **/
void bulletproof_input(size_t type, void* read_variable);


// METHOD-LIKE FUNCTIONS FOR THE BATTLESHIP GAME
/**
 * Reserves memory space for a board of the given dimensions, initializes it with WATER and the ships
 * and returns it.
 * @return pointer to the initialized board with dimensions DIM x DIM
 */
char* startDefenseBoardAuto();


/**
 * Reserves memory space for a board of the given dimensions, initializes it with WATER and asks the user to place his
 * ship while there are ships remaining. The ships must be placed correctly or the program will show an error message to
 * the user to try again.
 * @return pointer to the initialized board with dimensions DIM x DIM
 */
char* startDefenseBoardManual();


/**
 * Reserves memory space for a board of the given dimensions, initializes it with NOT_DISCOVERED_CELL and returns it.
 * @return
 */
char* startAttackBoard();


/**
 * Locates the limit of the ship pointed by the coordinates x and y and then checks
 * if all the positions of the ship are shot, then return true; otherwise false
 * @param board
 * @param dim
 * @param x
 * @param y
 * @return
 */
bool isSunk(char* board, unsigned char x, unsigned char y);


/**
 * Fills with water all the positions surrounding the ship located by the coordinates x and y
 * @param board
 * @param x
 * @param y
 */
void floodSorroundings(char* board, unsigned char x, unsigned char y);


/**
 * Translates a letter representing a column of the board to the corresponding
 * integer to index in the array. For example 'A' --> 0, 'B' --> 1, ..., 'X' --> dim - 1
 * @param column
 * @return
 */
unsigned char translateColumn(unsigned char column);


/**
 * Prints the board given by parameter.
 * @param board
 */
void showBoard(char* board);


/**
 * This functions updates the ship board with a shoot on a certain cell and informs the result of the shot:
 *  - Repeated cell (0)
 *  - Water (1)
 *  - Shot (2)
 *  - Shot & Sunk (3)
 * @param board
 * @param x
 * @param y
 * @return
 */
unsigned char shoot(char* board, unsigned char x, unsigned char y);


/**
 * Returns two coordinates representing a cell that has been targeted as the next shot.
 * This function tries to optimize the number of calls needed to this function in order to finish the game as soon
 * as possible.
 *
 * Static parameters:
 * static unsigned int state;
 * static unsigned int tried_state;
 *
 * @param board
 * @param dim
 */
void computeNextMovement(char* board, unsigned char* x, unsigned char* y);


/**
 * Calculates the final points for the info of the shoots of a certain player.
 * @param tableResultMoves
 * @return
 */
int calculateScore(DoubleLinkedList tableResultMoves);


/**
 * Locates a Ship pointed by the variables *x_ini and *y_ini by saving in *x_ini and *y_ini the initial coordinates of the ship
 * and saving in *x_end and *y_end the final coordinates of the ship.
 * This function expects correct coordinates.
 * This function always places the coordinates with lower values in *x_ini and *y_ini
 **/ 
void locateShip(char* board, unsigned char* x_ini, unsigned char* y_ini, unsigned char* x_end, unsigned char* y_end);


/**
 * Retrieves records from binary file and fills in the records List
 * @param records
 */
void loadRecords(DoubleLinkedList* records);


/**
 * Save records to binary file
 * @param records
 */
void saveRecords(DoubleLinkedList records);