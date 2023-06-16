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
#define RESULT_INITIAL 4

// Mask states
#define STATE_SEEK 0x00
#define STATE_DESTROY 0x10
#define STATE_VERTICAL 0x1C
#define STATE_HORIZONTAL 0x18
#define STATE_UP 0x1F
#define STATE_DOWN 0x1E
#define STATE_RIGHT 0x1B
#define STATE_LEFT 0x1A
#define STATE_MASK_MODE 0x10
#define STATE_MASK_ORIENTATION_DETECTED 0x08
#define STATE_MASK_ORIENTATION 0x04
#define STATE_MASK_DIRECTION_DETECTED 0x02
#define STATE_MASK_DIRECTION 0x01

// Magic numbers
// Number of tries when fitting a ship into the board before reinitializing the board again
#define AUTO_SHIP_PLACEMENT_MAX_TRIES 100000
// Maximum number of elements in an input array
#define MAX_READ_CHARACTERS 1000
// Proportion of positions used by ships in the beard
#define BOARD_USAGE_PERCENTAGE 20
// Maximum number of characters in the input to be read
#define MAX_CHAR_USER_INPUT 100

// Number of Players
#define ZERO_PLAYERS 0
#define ONE_PLAYER 1
#define TWO_PLAYERS 2


// CONSTANT VALUES
/**
 * Defines the number of ships for each ship of different size.
 * For example, NUM_SHIPS_TYPE[0] gives the number of ships of size 1
 * that we are going to have in the game.
 **/
unsigned char NUM_SHIPS_BY_SIZE[] = {4, 3, 2, 1};
const unsigned char SHIP_MAX_SIZE = 4;

// TYPE DEFINITION
typedef struct Position {
    unsigned int x;
    unsigned int y;
} Position;

    /*
     * Register that uses the 5 bits of less weight.
     * - The bit 4 (mask 0x10) at 0 or 1 determines if we are in SEEK mode or in DESTROY mode, respectively. In SEEK
     *   mode we shoot randomly until a ship is hit, and we enter in DESTROY mode. In DESTROY mode we try to sink the
     *   ship that made us enter in DESTROY mode, and we will enter again in SEEK mode if we destroy the ship.
     * - The bit 3 (mask 0x08) at 1 or 0 determines if we have detected the orientation of the ship or not.
     * - The bit 2 (mask 0x04) at 1 or 0 determines if the ship is vertical or horizontal. This value is valid only if
     *   the bit 3 is at 1.
     * - The bit 1 (mask 0x02) at 1 or 0 determines if we have detected a unique direction to sink the ship.
     * - The bit 0 (mask 0x01) at 1 or 0 determines if we need to go RIGHT / UP or LEFT / DOWN, depending on the
     *   corresponding value of bit 2.
     */
    //unsigned char mode;

typedef struct Player {
    char** attackBoard;  // Points to the board containing the revealed positions of the enemy
    char** defenseBoard;  // Points to the board that contains our ships
    DoubleLinkedList tableResultMoves;  // Contains all the results from the shots of this player
    Position lastShot;
    unsigned int lastResult;
    unsigned int shot_ships;
} Player;

typedef struct Game {
    unsigned int num_players;  // Contains the number of players.
    Player players[2];
    unsigned int dim;
} Game;

// GLOBAL VARIABLE DEFINITIONS
extern unsigned int DIM;  // Contains the dimension of the board. Accessible globally

// PROCEDURE-LIKE (STATIC) FUNCTIONS
/**
 * Reads safely from user input a variable of the type specified by the type parameter.
 * Forces the user to introduce a valid input. Stores data read in read_variable.
 **/
// void* bulletproof_input(size_t type, const int* num_elements, char separator);

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
 * This function always places the coordinates with lower (or equal) values in *x_ini and *y_ini
 **/
void locateShip(char** board, Position* position_ini, Position* position_end);

/**
 * Detects the orientation of a ship in the board. Returns 0 if no orientation is detected, 1 if the ship is vertical
 * and 2 if the ship is horizontal. The function assumes that there is a ship in the received position.
 * @param board
 * @param shipPosition
 * @return
 */
unsigned int detectOrientation(char** board, Position shipPosition);

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
Position computeNextMovement(char** attach_board, Position lastShot, unsigned int result_last_shot);

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

/**
 * Modifies *NUM_SHIPS_BY_SIZE to fit the BOARD_USAGE_PERCENTAGE.
 * This function ensures that the board usage percentage is equal or greater than BOARD_USAGE_PERCENTAGE.
*/
void satisfyUsagePercentage();


/**
 * Tries to read an int from the user input until it achieves it.
*/
int readInt();


/**
 * Tries to read an integer in a given range from user input until the range is satisfied.
 * The range goes from the minimum number to the maximum number including both of them.
*/
int readIntInRange(int minimumNumber, int maximumRange);


/**
 * Display the menu options in the screen
*/
void showMenu();

/**
 * Display the menu options in the screen
*/
int readMenuEntry();

/**
 * Display the menu options in the screen
*/
void startNewGame();

/**
 * Display the menu options in the screen
*/
void annotateLastShoot(char** attackBoard, unsigned int lastResult, Position lastShot);

/**
 * Returns the number of boats in num ships by size
*/
unsigned int getNumberOfBoats();

int inputBoardDimension();

void play();

void initializePlayer(Player* player);

void readChar();