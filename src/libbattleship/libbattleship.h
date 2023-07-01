// DYNAMIC INCLUDES
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// STATIC INCLUDES
#include "../libinput/libinput.h"
#include "../libdoublelinkedlist/libdoublelinkedlist.h"

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
#define RESULT_ERROR -1

// Result of a game
#define PLAYER0_WINS 0
#define PLAYER1_WINS 1
#define PAUSED_GAME -1

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

// Number of Players
#define ZERO_PLAYERS 0
#define ONE_PLAYER 1
#define TWO_PLAYERS 2

// COIN VALUES
#define HEADS 1
#define TAILS 0

// Magic numbers
// Number of tries when fitting a ship into the board before reinitializing the board again
#define AUTO_SHIP_PLACEMENT_MAX_TRIES 100000
// Proportion of positions used by ships in the board
#define BOARD_USAGE_PERCENTAGE 20

// Error code for translate functions
#define ERROR_PARAMETER_OUT_OF_RANGE -1


// CONSTANTS
// Default number of ships by size.
// unsigned char defaultNumShipsBySize[4];
// unsigned char defaultShipMaxSize;  // Dimension of the NUM_SHIPS_BY_SIZE array


// TYPE DEFINITION
// Position in the board
typedef struct Position {
    unsigned int x;
    unsigned int y;
} Position;

// Contains all the data of a player
typedef struct Player {
    char** attackBoard;  // Points to the board containing the revealed positions of the enemy
    char** defenseBoard;  // Points to the board that contains our ships
    Position lastShot;
    unsigned int lastResult;
    unsigned int shot_ships;
    bool isHuman;  // True if the actions of the player have to be controlled with user input
    int score;
    unsigned int totalShots;
} Player;

// Contains the data of an initialized game
typedef struct Game {
    unsigned int num_players;  // Contains the number of players.
    Player players[2];
    unsigned int dim;
    /*
     * Defines the number of ships for each ship of different size. For example, NUM_SHIPS_TYPE[0] gives the number of
     * ships of size 1 that we are going to have in the game.
     */
    unsigned char* numShipsBySize;
    unsigned char shipMaxSize;  // Dimension of the NUM_SHIPS_BY_SIZE array
    bool isGameInitialized;  // True: if the game is initialized and can be played; False: Otherwise.
} Game;


// Contains the data of a Record
typedef struct Record {
    char* playerName;
    int score;
} Record;


// PROCEDURE-LIKE (STATIC) FUNCTIONS
/**
 * Calculates the natural logarithm of a number x with a given base b
 * @param x Entry of the logarithm
 * @param b Base of the logarithm
 * @return Result of the logarithm
 */
unsigned int naturalLog(unsigned int x, unsigned int b);

/**
 * Custom implementation of memcopy. It copies n bytes from the src array to the dest array.
 *
 * @param dest Destination array
 * @param src Source array.
 * @param n Number of bytes to be copied.
 * @return Pointer to the destiny.
 */
void* memcpy(void* dest, const void* src, size_t n);

// METHOD-LIKE FUNCTIONS FOR THE BATTLESHIP GAME


 /**
  * Reserves memory for a board and returns the pointer to the board.
  * @param dim
  * @return Defense board.
  */
char** reserveBoard(unsigned int dim);

/**
 * Fills the received board with NOT_DISCOVERED symbols.
 * @param board
 */
void initializeBoard(char** board, unsigned int dim);

/**
 *
 * @param board game board
 * @return true if it is correctly initialized and false if is not completely initialized
 */
 /**
  * Tries to initialize the board with all the ships and returns true if has been initialized correctly and false if the
  * board is not completely initialized.
  *
  * @param defense_board Board where the user puts its own boats
  * @param dim Dimension of the matrix
  * @param numShipsBySize Array that contains the number of ships a of a certain type
  * @param shipMaxSize
  * @return
  */
bool initializeBoardWithShipsAutoPrivate(char** defense_board, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize);

/**
 * Reserves memory space for a board of the given dimensions, initializes it with WATER and the ships
 * and returns it.
 * @return pointer to the initialized board with dimensions DIM x DIM
 */
void initializeBoardWithShipsAuto(char** board, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize);

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
void floodSurroundings(char** board, Position shipPosition, unsigned int dim);

/**
 * Locates a Ship pointed by the variables *x_ini and *y_ini by saving in *x_ini and *y_ini the initial coordinates of the ship
 * and saving in *x_end and *y_end the final coordinates of the ship.
 * This function expects correct coordinates.
 * This function always places the coordinates with lower (or equal) values in *x_ini and *y_ini
 **/
void locateShip(char** board, Position* position_ini, Position* position_end, unsigned int dim);

/**
 * Detects the orientation of a ship in the board. Returns 0 if no orientation is detected, 1 if the ship is vertical
 * and 2 if the ship is horizontal. The function assumes that there is a ship in the received position.
 * @param board
 * @param shipPosition
 * @return
 */
unsigned int detectOrientation(char** board, Position shipPosition, unsigned int dim);

/**
 * Locates the limit of the ship pointed by the coordinates x and y and then checks
 * if all the positions of the ship are shot, then return true; otherwise false
 * @param board
 * @param dim
 * @param position
 * @return
 */
bool isSunk(char** board, Position position, unsigned int dim);

/**
 * Returns true if the ship specified by parameters does not collide wth any elements of the given board.
 * It also returns the computed end position of the ship
 * @param defense_board
 * @param position
 * @param ship_size
 * @param orientation
 * @return
 */
bool doesFit(char** defense_board, Position position, unsigned int ship_size, bool orientation, unsigned int dim);

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
unsigned int shoot(char** board, Position position, unsigned int dim);

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
Position computeNextMovement(char** attach_board, Position lastShot, unsigned int result_last_shot, unsigned int dim);

/**
 * Prints the board given by parameter.
 * @param board
 */
void showBoard(char** board, unsigned int dim);

/**
 * Modifies *NUM_SHIPS_BY_SIZE to fit the BOARD_USAGE_PERCENTAGE.
 * This function ensures that the board usage percentage is equal or greater than BOARD_USAGE_PERCENTAGE.
*/
void satisfyUsagePercentage(unsigned char* numShipsBySize, unsigned char shipMaxSize, unsigned int dim);

/**
 * Display the menu options in the screen
 * game.isGameInitialized boolean value as an argument
*/
void showMenu(bool isGameInitialized);

/**
 * Display the menu options in the screen
*/
int readMenuEntry(bool isGameInitialized);

/**
 * Display the menu options in the screen
*/
void startNewGame();

/**
 * Display the menu options in the screen
*/
void annotateLastShoot(char** attackBoard, unsigned int lastResult, Position lastShot, unsigned int dim);

/**
 * Returns the number of boats in num ships by size
*/
unsigned int getNumberOfBoats(unsigned char* numShipsBySize, unsigned char shipMaxSize);

/**
 * Performs the algorithm of the game depending on the type of game passed by parameter. Returns 1 if the game is
 * finished and the player one has won, Returns 0 if the game is finished and the player zero has won and
 * Returns -1 if the game is Paused.
 *
 * @param game Instance of the game.
 * @return Returns false if the game is not finished and true if it is.
 */
int play(Game* game);

/**
 * This function simulates a turn with a player and works as an helper function for the play function
 *
 * @param game
 * @param playerNumber
 * @param isPlayerOneTurn
 * @return
 */
int playTurn(Game* game, unsigned int playerNumber, bool* isPlayerOneTurn);

int inputBoardDimension();

void initializePlayer(Player* player, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize);

void initializeGame(Game* game);

/**
  * You send a char representing a column of the board and returns the correspondant position to index in the array of
  the board. Returns ERROR_PARAMETER_OUT_OF_RANGE if the letter does not exist for the supplied dimension.
*/
int columnToIndex(char letter, unsigned int dim);

/**
  * It receives an int that indexes the columns of the board. It returns the character representing the column for the
  received index. Returns ERROR_PARAMETER_OUT_OF_RANGE if the index does not exist for the supplied dimension.
*/
char indexToColumn(int index, unsigned int dim);


/**
  * You send a int representing a row of the board and returns the correspondant position to index in the array of
  the board. Returns ERROR_PARAMETER_OUT_OF_RANGE if the row does not exist for the supplied dimension.
*/
int rowToIndex(int row, unsigned int dim);

/**
  * It receives an int that indexes the rows of the board. It returns the integer representing the row for the
  received index. Returns ERROR_PARAMETER_OUT_OF_RANGE if the index does not exist for the supplied dimension.
*/
int indexToRow(int index, unsigned int dim);

/**
 * It asks for head or tails to flip a coin and tell you if you win or lost.
*/
bool throwCoin();


DoubleLinkedList* initializeRecords();


/**
 * Calculates the final points for the info of the shoots of a certain player.
 * @param tableResultMoves
 * @return
 */
//int calculateScore(DoubleLinkedList tableResultMoves);


void addRecord(DoubleLinkedList* records, Record record);

void showRecord(void * record);

int compareRecords(void * recordA, void * recordB);

void showRecords(DoubleLinkedList records);

/**
 * Retrieves records from binary file and fills in the records List
 * @param records
 */
//void loadRecords(DoubleLinkedList* records);


/**
 * Save records to binary file
 * @param records
 */
//void saveRecords(DoubleLinkedList records);