// Includes
#include <stdio.h>
#include <stdbool.h>  // To use booleans
#include <stdlib.h>

#define NOT_DISCOVERED_CELL '?'
#define WATER '~'
#define SHOT_WATER '-'
#define SHIP '@'
#define SHOT_SHIP 'X'

// Defines the number of ships for each ship of different size. 
// For example, NUM_SHIPS_TYPE[0] gives the number of ships of size 1 
// that we are going to have in the game
const unsigned char NUM_SHIPS_BY_SIZE = {4, 3, 2, 1};
const unsigned char SHIP_MAX_SIZE = 4;

/**
 * Locates a Ship pointed by the variables *x_ini and *y_ini by saving in *x_ini and *y_ini the initial coordinates of the ship
 * and saving in *x_end and *y_end the final coordinates of the ship.
 * This function expects correct coordinates.
 **/ 
void locateShip(char* board, unsigned char* x_ini, unsigned char* y_ini, unsigned char* x_end, unsigned char* y_end);


/**
 * Reads safely from user input a variable of the type specified by the type parameter.
 * Forces the user to introduce a valid input. Stores data read in readed_variable.
 **/
void bulletproof_input(c_type type, void* readed_variable);
