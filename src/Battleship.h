// INCLUDES
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "DoubleLinkedList.h"


// DEFINES
// Cell symbols
#define NOT_DISCOVERED_CELL '?'
#define WATER '~'
#define SHOT_WATER '-'
#define SHIP '@'
#define SHOT_SHIP 'X'

// Result of a shot
#define RESULT_REPEATED_CELL 0
#define RESULT_WATER 1
#define RESULT_SHOT 2
#define RESULT_SHOT_AND_SUNK 3

// Magic numbers
#define AUTO_SHIP_PLACEMENT_MAX_TRIES 100000


// CONSTANT VALUES
/**
 * Defines the number of ships for each ship of different size.
 * For example, NUM_SHIPS_TYPE[0] gives the number of ships of size 1
 * that we are going to have in the game.
 **/
const unsigned char NUM_SHIPS_BY_SIZE[] = {4, 3, 2, 1};
const unsigned char SHIP_MAX_SIZE = 4;


// TYPE DEFINITION
typedef struct Player {
    unsigned char** attackBoard;  // Points to the board containing the revealed positions of the enemy
    unsigned char** defenseBoard;  // Points to the board that contains our ships
    DoubleLinkedList* tableResultMoves;  // Contains all the results from the shots of this player
} Player;

typedef struct Position {
    unsigned int x;
    unsigned int y;
} Position;


// GLOBAL VARIABLE DEFINITIONS
extern unsigned int DIM;  // Contains the dimension of the board. . Most of functions use this variable


// PROCEDURE-LIKE (STATIC) FUNCTIONS
/**
 * Reads safely from user input a variable of the type specified by the type parameter.
 * Forces the user to introduce a valid input. Stores data read in readed_variable.
 **/
void ** bulletproof_input(size_t type, void* read_variable);

/**
 * Calculates the natural logarithm of a number x with a given base b
 * @param x Entry of the logarithm
 * @param b Base of the logarithm
 * @return Result of the logarithm
 */
unsigned int naturalLog(unsigned int x, unsigned int b);

/**
 * Pauses the code
 */
void pause();


// METHOD-LIKE FUNCTIONS FOR THE BATTLESHIP GAME

/**
 * Reserves memory for a board and returns the pointer to the board.
 * @return new board
 */
char** reserveBoard();

/**
 * Fills the received board with NOT_DISCOVERED symbols.
 * @param board
 */
void initializeBoard(char** board);

/**
 * Reserves memory space for a board of the given dimensions, initializes it with WATER and the ships
 * and returns it.
 * @return pointer to the initialized board with dimensions DIM x DIM
 */
void initializeBoardWithShipsAuto(char** board);

/**
 * Reserves memory space for a board of the given dimensions, initializes it with WATER and asks the user to place his
 * ship while there are ships remaining. The ships must be placed correctly or the program will show an error message to
 * the user to try again.
 * @return pointer to the initialized board with dimensions DIM x DIM
 */
void initializeBoardWithShipsManual(char** board);

/**
 * Initializes a new ship in the board given its begin and end positions
 * @param defense_board
 * @param x_ini
 * @param y_ini
 * @param x_end
 * @param y_end
 */
void initializeShip(char** defense_board, Position position, unsigned int ship_size, bool orientation);

/**
 * Fills with water all the positions surrounding the ship located by the coordinates x and y
 * @param board
 * @param shipPosition
 */
void floodSurroundings(char** board, Position shipPosition);

/**
 * Locates a Ship pointed by the variables *x_ini and *y_ini by saving in *x_ini and *y_ini the initial coordinates of the ship
 * and saving in *x_end and *y_end the final coordinates of the ship.
 * This function expects correct coordinates.
 * This function always places the coordinates with lower values in *x_ini and *y_ini
 **/
void locateShip(char** board, Position* position_ini, Position* position_end);

/**
 * Locates the limit of the ship pointed by the coordinates x and y and then checks
 * if all the positions of the ship are shot, then return true; otherwise false
 * @param board
 * @param dim
 * @param position
 * @return
 */
bool isSunk(char** board, Position position);

/**
 * Returns true if the ship specified by parameters does not collide wth any elements of the given board.
 * It also returns the computed end position of the ship
 * @param defense_board
 * @param position
 * @param ship_size
 * @param orientation
 * @return
 */
bool doesFit(char** defense_board, Position position, unsigned int ship_size, bool orientation);

/**
 * This functions updates the ship board with a shoot on a certain cell and informs the result of the shot:
 *  - Repeated cell (0)
 *  - Water (1)
 *  - Shot (2)
 *  - Shot & Sunk (3)
 * @param board
 * @param position
 * @return
 */
unsigned int shoot(char** board, Position position);

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
void computeNextMovement(char** board, Position* position, int result_last_shot);

/**
 * Calculates the final points for the info of the shoots of a certain player.
 * @param tableResultMoves
 * @return
 */
int calculateScore(DoubleLinkedList tableResultMoves);

/**
 * Prints the board given by parameter.
 * @param board
 */
void showBoard(char** board);

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