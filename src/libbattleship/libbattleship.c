#include "libbattleship.h"

// PROCEDURE-LIKE (STATIC) FUNCTIONS
unsigned int naturalLog(unsigned int x, unsigned int base)
{
    unsigned int nlog = 0;
    for (unsigned int i = x; i > 0; i /= base)
        nlog++;
    return nlog - 1;
}

void* memcpy(void* dest, const void* src, size_t n)
{
    // Initialize pointers with implicit size of a byte
    char* src_byte = (char*) src;
    char* dest_byte = (char*) dest;

    for (unsigned int i = 0; i < n; i++)
    {
        dest_byte[i] = src_byte[i];
    }
    return dest;
}


// METHOD-LIKE FUNCTIONS FOR THE BATTLESHIP GAME
char** reserveBoard(unsigned int dim)
{
    // Initialize an array to an array of characters
    char** board = malloc(dim * sizeof(char*));

    // Initialize each array of characters
    for (unsigned int i = 0; i < dim; i++)
        board[i] = malloc(dim * sizeof(char));

    return board;
}

void initializeBoard(char** board, unsigned int dim)
{
    for (unsigned int i = 0; i < dim; i++)
        for (unsigned int j = 0; j < dim; j++)
            board[i][j] = NOT_DISCOVERED_CELL;
}

/**
 * Tries to initialize the board with all the ships and returns true if has been initialized correctly and false if the
 * board is not completely initialized.
 * @param board game board
 * @return true if it is correctly initialized and false if is not completely initialized
 */
bool initializeBoardWithShipsAutoPrivate(char** defense_board, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize)
{
    Position ini;
    unsigned int orientation = 0, number_of_tries;

    // Loop through each ship size
    for (unsigned int ship_size = 1; ship_size <= shipMaxSize; ship_size++)
    {
        // Loop through each individual ship
        for (unsigned int ship_counter = 0; ship_counter < numShipsBySize[ship_size - 1]; ship_counter++)
        {
            number_of_tries = 0;
            // Try to put the current ship on the board
            while (true)
            {
                // Generate random position and orientation to put the current ship
                ini.x = rand() % dim;
                ini.y = rand() % dim;
                if (ship_size > 1)
                    orientation = rand() % 2;

                // If it does fit, put it and go to the next ship
                if (doesFit(defense_board, ini, ship_size, orientation, dim))
                {
                    initializeShip(defense_board, ini, ship_size, orientation);
                    floodSurroundings(defense_board, ini, dim);
                    break;
                }
                else  // If it does not fit, annotate another try. If we go through the max tries values, return false
                {
                    number_of_tries++;
                    if (number_of_tries > AUTO_SHIP_PLACEMENT_MAX_TRIES)
                        return false;
                }
            }
        }
    }
    return true;
}

void initializeBoardWithShipsAuto(char** board, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize)
{
    // While the board can not be initialized, keep trying
    do
        initializeBoard(board, dim);
    while (!initializeBoardWithShipsAutoPrivate(board, dim, numShipsBySize, shipMaxSize));

    // When the board is initialized, substitute not discovered cells with water
    for (unsigned int i = 0; i < dim; i++)
        for (unsigned int j = 0; j < dim; j++)
            if (board[i][j] == NOT_DISCOVERED_CELL)
                board[i][j] = WATER;
}

void initializeShip(char** defense_board, Position ini, unsigned int ship_size, bool orientation)
{
    Position end;
    if (orientation)
    {
        end.x = ini.x + ship_size - 1;
        end.y = ini.y;
    }
    else
    {
        end.y = ini.y + ship_size - 1;
        end.x = ini.x;
    }

    for (unsigned int i = ini.x; i <= end.x; i++)
        for (unsigned int j = ini.y; j <= end.y; j++)
            defense_board[i][j] = SHIP;
}

void floodSurroundings(char** board, Position position, unsigned int dim)
{
    Position end;

    locateShip(board, &position, &end, dim);

    if (position.x > 0) position.x--;
    if (position.y > 0) position.y--;
    if (end.x < dim - 1) end.x++;
    if (end.y < dim - 1) end.y++;

    for (unsigned int i = position.x; i <= end.x; i++)
        for (unsigned int j = position.y; j <= end.y; j++)
            if (board[i][j] != SHOT_SHIP && board[i][j] != SHIP)
                board[i][j] = WATER;
}

void locateShip(char** board, Position* position_ini, Position* position_end, unsigned int dim)
{
    // Initialize end with the position of reference
    position_end->x = position_ini->x;
    position_end->y = position_ini->y;

    // Move position x end further to the end if a SHIP found
    while (position_end->x + 1 < dim && (board[position_end->x + 1][position_end->y] == SHIP || board[position_end->x + 1][position_end->y] == SHOT_SHIP))
        position_end->x++;

    // Move position y end further to the end if a SHIP found
    while (position_end->y + 1 < dim && (board[position_end->x][position_end->y + 1] == SHIP || board[position_end->x][position_end->y + 1] == SHOT_SHIP))
        position_end->y++;

    // Move position x ini further to the beginning if a SHIP found
    while (position_ini->x > 0 && (board[position_ini->x - 1][position_ini->y] == SHIP || board[position_ini->x - 1][position_ini->y] == SHOT_SHIP))
        position_ini->x--;

    // Move position y ini further to the beginning if a SHIP found
    while (position_ini->y > 0 && (board[position_ini->x][position_ini->y - 1] == SHIP || board[position_ini->x][position_ini->y - 1] == SHOT_SHIP))
        position_ini->y--;
}

unsigned int detectOrientation(char** board, Position shipPosition, unsigned int dim)
{
    // First detect any other ship position in the surroundings
    if (shipPosition.x > 0 && (board[shipPosition.x - 1][shipPosition.y] == SHOT_SHIP || board[shipPosition.x - 1][shipPosition.y] == SHIP))
        return 2;
    else if (shipPosition.y > 0 && (board[shipPosition.x][shipPosition.y - 1] == SHOT_SHIP || board[shipPosition.x][shipPosition.y - 1] == SHIP))
        return 1;
    else if (shipPosition.x < (dim - 1) && (board[shipPosition.x + 1][shipPosition.y] == SHOT_SHIP || board[shipPosition.x + 1][shipPosition.y] == SHIP))
        return 2;
    else if (shipPosition.y < (dim - 1) && (board[shipPosition.x][shipPosition.y + 1] == SHOT_SHIP || board[shipPosition.x][shipPosition.y + 1] == SHIP))
        return 1;

    /*
     * Then we check the cases where there is water, shot water or board limits on both sides, assuming that the ship
     * has just one point discovered, but it is longer.
     */
    if ((shipPosition.x == 0 || board[shipPosition.x - 1][shipPosition.y] != NOT_DISCOVERED_CELL) && (shipPosition.x == (dim - 1) || board[shipPosition.x + 1][shipPosition.y] != NOT_DISCOVERED_CELL))
        return 1;
    else if ((shipPosition.y == 0 || board[shipPosition.x][shipPosition.y - 1] != NOT_DISCOVERED_CELL) && (shipPosition.y == (dim - 1) || board[shipPosition.x][shipPosition.y + 1] != NOT_DISCOVERED_CELL))
        return 2;
    return 0;
}

bool isSunk(char** board, Position position, unsigned int dim)
{
    Position end;

    locateShip(board, &position, &end, dim);

    if (position.x > 0)
        position.x--;
    if (end.x < dim - 1)
        end.x++;
    if (position.y > 0)
        position.y--;
    if (end.y < dim - 1)
        end.y++;

    for (unsigned int i = position.x; i <= end.x; i++)
    {
        for (unsigned int j = position.y; j <= end.y; j++)
        {
            if (board[i][j] == NOT_DISCOVERED_CELL || board[i][j] == SHIP)
                return false;
        }
    }
    return true;
}

bool doesFit(char** defense_board, Position ini, unsigned int ship_size, bool orientation, unsigned int dim)
{
    Position end;

    // Compute theoretical limits
    if (orientation)
    {
        end.x = ini.x + ship_size - 1;
        end.y = ini.y;
    }
    else
    {
        end.y = ini.y + ship_size - 1;
        end.x = ini.x;
    }

    // If we went out of bounds of the board when calculating the limits does not fit
    if (end.x >= dim || end.y >= dim)
        return false;

    // If there are any occupied cells in the limits it does not fit
    for (unsigned int i = ini.x; i <= end.x; i++)
        for (unsigned int j = ini.y; j <= end.y; j++)
            if (defense_board[i][j] != NOT_DISCOVERED_CELL)
                return false;

    return true;
}

unsigned int shoot(char** board, Position position, unsigned int dim)
{
    switch (board[position.x][position.y])
    {
        case SHOT_WATER:
        {
            return 0;
        }
        case WATER:
        {
            board[position.x][position.y] = SHOT_WATER;
            return 1;
        }
        case SHIP:
        {
            board[position.x][position.y] = SHOT_SHIP;
            if (isSunk(board, position, dim))
                return 3;
            else
                return 2;
        }
        default:
        {
            return -1;
        }
    }
}

void computeNextMovementDetectState(char** attack_board, Position iniCurrentShipPosition, unsigned int* state, unsigned int dim)
{
    Position endCurrentShipPosition;
    // Now we need to obtain info about orientation
    unsigned int orientation = detectOrientation(attack_board, iniCurrentShipPosition, dim);

    if (! orientation)
    {
        *state = STATE_DESTROY;
    }
    else if (orientation == 1)
    {
        locateShip(attack_board, &iniCurrentShipPosition, &endCurrentShipPosition, dim);
        if (endCurrentShipPosition.y == dim - 1 || attack_board[endCurrentShipPosition.x][endCurrentShipPosition.y + 1] != NOT_DISCOVERED_CELL)
        {
            *state = STATE_DOWN;
        }
        else if (endCurrentShipPosition.y == 0 || attack_board[endCurrentShipPosition.x][endCurrentShipPosition.y - 1] != NOT_DISCOVERED_CELL)
        {
            *state = STATE_UP;
        }
        else
        {
            *state = STATE_VERTICAL;
        }
    }
    else if (orientation == 2)
    {

        locateShip(attack_board, &iniCurrentShipPosition, &endCurrentShipPosition, dim);
        if (endCurrentShipPosition.x == dim - 1 || attack_board[endCurrentShipPosition.x + 1][endCurrentShipPosition.y] != NOT_DISCOVERED_CELL)
        {
            *state = STATE_LEFT;
        }
        else if (endCurrentShipPosition.x == 0 || attack_board[endCurrentShipPosition.x - 1][endCurrentShipPosition.y] != NOT_DISCOVERED_CELL)
        {
            *state = STATE_RIGHT;
        }
        else
        {
            *state = STATE_HORIZONTAL;
        }
    }
}

void generateSurroundingPositions(Position iniPosition, Position* surroundingPositions, unsigned int* surroundingPositionsSize, unsigned int dim)
{
    unsigned int surroundingCandidatePositionsLength = 0;

    // Check directions starting from iniPosition
    if (iniPosition.x < dim - 1)
    {
        surroundingPositions[surroundingCandidatePositionsLength] = iniPosition;
        surroundingPositions[surroundingCandidatePositionsLength].x++;
        surroundingCandidatePositionsLength++;
    }
    if (iniPosition.y > 0)
    {
        surroundingPositions[surroundingCandidatePositionsLength] = iniPosition;
        surroundingPositions[surroundingCandidatePositionsLength].y--;
        surroundingCandidatePositionsLength++;
    }
    if (iniPosition.x > 0)
    {
        surroundingPositions[surroundingCandidatePositionsLength] = iniPosition;
        surroundingPositions[surroundingCandidatePositionsLength].x--;
        surroundingCandidatePositionsLength++;
    }
    if (iniPosition.y < dim - 1)
    {
        surroundingPositions[surroundingCandidatePositionsLength] = iniPosition;
        surroundingPositions[surroundingCandidatePositionsLength].y++;
        surroundingCandidatePositionsLength++;
    }
    *surroundingPositionsSize = surroundingCandidatePositionsLength;
}


void discardDiscovered(char** attack_board, Position* surroundingPositions, unsigned int* surroundingPositionsSize)
{
    Position result[*surroundingPositionsSize];
    unsigned int resultSize = 0;

    // Loop through all the positions and add the not discovered to a temporal array.
    for (unsigned int i = 0; i < *surroundingPositionsSize; i++)
    {
        // Discard all discovered cells
        if (attack_board[surroundingPositions[i].x][surroundingPositions[i].y] == NOT_DISCOVERED_CELL)
        {
            result[resultSize] = surroundingPositions[i];
            resultSize++;
        }
    }

    // Fill the array and the integer of the num elements
    for (unsigned int i = 0; i < resultSize; i++)
    {
        surroundingPositions[i] = result[i];
    }
    *surroundingPositionsSize = resultSize;
}



Position computeNextMovement(char** attack_board, Position lastShotPosition, unsigned int result_last_shot, unsigned int dim)
{
    unsigned int state, surroundingCandidatePositionsLength;
    Position surroundingPositions[4], iniCurrentShipPosition;

    // First detect the previous state from the result
    if (result_last_shot == RESULT_SHOT_AND_SUNK || result_last_shot == RESULT_INITIAL)
    {
        // We know that we are in the initial mode
        state = STATE_SEEK;
    }
    else if (result_last_shot == RESULT_WATER)
    {
        // Get surrounding positions from the last shot, so we can find the current ship
        generateSurroundingPositions(lastShotPosition, surroundingPositions, &surroundingCandidatePositionsLength, dim);

        // We need to check the four surrounding cells for a shot but not sunk ship
        unsigned int i = 0;
        while ((i < surroundingCandidatePositionsLength)
                && ((attack_board[surroundingPositions[i].x][surroundingPositions[i].y] != SHOT_SHIP)
                || (attack_board[surroundingPositions[i].x][surroundingPositions[i].y] == SHOT_SHIP
                && isSunk(attack_board,  surroundingPositions[i], dim))))
        {
            i++;
        }

        // If we arrived to the end it means we did not find a surrounding ship
        if (i == surroundingCandidatePositionsLength)
        {
            state = STATE_SEEK;
        }
        else
        {
            // We did find a not destroyed ship in surroundingPositions[i], detect state from that ship
            iniCurrentShipPosition = surroundingPositions[i];
            computeNextMovementDetectState(attack_board, iniCurrentShipPosition, &state, dim);
        }
    }
    else if (result_last_shot == RESULT_SHOT)
    {
        // Detect state using the position of the last shot ship
        computeNextMovementDetectState(attack_board, lastShotPosition, &state, dim);
        // We are already in a ship, so the iniCurrentShipPosition will be the lastShotPosition
        iniCurrentShipPosition = lastShotPosition;
    }

    // At this point iniCurrentShipPosition points to a not sunk ship
    // Decide position depending on the state
    switch (state) {

        // Generate a random valid cell and continue
        case STATE_SEEK:
        {
            Position result;
            do
            {
                result.x = rand() % dim;
                result.y = rand() % dim;
            }
            while (attack_board[result.x][result.y] != NOT_DISCOVERED_CELL);
            return result;
        }

        // In this state we need to choose randomly between one of the valid surrounding positions
        case STATE_DESTROY:
        {
            // Get surrounding positions from the visible point of the ship. We can reuse variables
            generateSurroundingPositions(iniCurrentShipPosition, surroundingPositions, &surroundingCandidatePositionsLength, dim);

            // Discard the discovered cells
            discardDiscovered(attack_board, surroundingPositions, &surroundingCandidatePositionsLength);

            // Return a random surrounding not discovered position
            return surroundingPositions[rand() % surroundingCandidatePositionsLength];
        }

        // We need to choose between the up or down position of the ship.
        case STATE_VERTICAL:
        {
            // Generate down position
            surroundingPositions[0] = iniCurrentShipPosition;
            do
            {
                surroundingPositions[0].y--;
            }
            while (attack_board[surroundingPositions[0].x][surroundingPositions[0].y] == SHOT_SHIP);

            // Generate up position
            surroundingPositions[1] = iniCurrentShipPosition;
            do
            {
                surroundingPositions[1].y++;
            }
            while (attack_board[surroundingPositions[1].x][surroundingPositions[1].y] == SHOT_SHIP);

            // Choose randomly between the two positions
            return surroundingPositions[rand() % 2];
        }

        // We need to choose between the left or right position of the ship.
        case STATE_HORIZONTAL:
        {
            // Generate left position
            surroundingPositions[0] = iniCurrentShipPosition;
            do
            {
                surroundingPositions[0].x--;
            }
            while (attack_board[surroundingPositions[0].x][surroundingPositions[0].y] == SHOT_SHIP);

            // Generate right position
            surroundingPositions[1] = iniCurrentShipPosition;
            do
            {
                surroundingPositions[1].x++;
            }
            while (attack_board[surroundingPositions[1].x][surroundingPositions[1].y] == SHOT_SHIP);

            // Choose randomly between the two positions
            return surroundingPositions[rand() % 2];
        }

        // We need to generate the most right position
        case STATE_RIGHT:
        {
            do
            {
                iniCurrentShipPosition.x++;
            }
            while (attack_board[iniCurrentShipPosition.x][iniCurrentShipPosition.y] == SHOT_SHIP);

            return iniCurrentShipPosition;
        }

        // We need to generate the most left position
        case STATE_LEFT:
        {
            do
            {
                iniCurrentShipPosition.x--;
            }
            while (attack_board[iniCurrentShipPosition.x][iniCurrentShipPosition.y] == SHOT_SHIP);

            return iniCurrentShipPosition;
        }

        // We need to generate the most down position
        case STATE_DOWN:
        {
            do
            {
                iniCurrentShipPosition.y--;
            }
            while (attack_board[iniCurrentShipPosition.x][iniCurrentShipPosition.y] == SHOT_SHIP);

            return iniCurrentShipPosition;
        }

        // We need to generate the most up position
        case STATE_UP:
        {
            do
            {
                iniCurrentShipPosition.y++;
            }
            while (attack_board[iniCurrentShipPosition.x][iniCurrentShipPosition.y] == SHOT_SHIP);

            return iniCurrentShipPosition;
        }

        default:
        {
            iniCurrentShipPosition.x = 0;
            iniCurrentShipPosition.y = 0;
            return iniCurrentShipPosition;
        }
    }
}

void showBoard(char** board, unsigned int dim)
{
    // Obtain the number of digits of DIM
    unsigned int DIM_num_digits = naturalLog(dim, 10) + 1;

    // Begin printing board in new line
	printf("\n");

    // Before printing the columns (A, B, C...) print as many spaces as the number of digits that DIM has + 1 for the
    // separation
	for (unsigned int i = 0; i < DIM_num_digits + 1; i++)
	    printf(" ");

    // Print the columns row, starting from A
	for (unsigned int i = 0; i < dim; i++)
		printf("%c", 'A' + i);

    // For each row
	for (unsigned int j = 0; j < dim; j++)
	{
        // Begin in a new line and print the current row j as number
		printf("\n%i", j + 1);
        // Compute how many digits has the current row number
		unsigned int current_row_num_digits = naturalLog(j + 1, 10) + 1;
        // Print as many spaces as needed to reach the num digits of DIM knowing the num digits of the current row num
        for (unsigned int i = 0; i < DIM_num_digits - current_row_num_digits + 1; i++)
            printf(" ");

        // After the spaces print the board content
		for (unsigned int i = 0; i < dim; i++)
			printf("%c", board[i][j]);
	}
    // End with newline
	printf("\n");
}

unsigned int computePositionsOccupied(unsigned char* numShipsBySize, unsigned char shipMaxSize)
{
    unsigned int numberPositionsUsed = 0;

    for (unsigned int i = 0; i < shipMaxSize; i++)
    {
        numberPositionsUsed += numShipsBySize[i] * (i + 1);
    }
    return numberPositionsUsed;
}


float computePercentageOccupancy(unsigned char* numShipsBySize, unsigned char shipMaxSize, unsigned int dim)
{
    return ((float) computePositionsOccupied(numShipsBySize, shipMaxSize) / (float) (dim * dim)) * 100;
}


void satisfyUsagePercentage(unsigned char* numShipsBySize, unsigned char shipMaxSize, unsigned int dim)
{
    unsigned int n = 0;
    while (computePercentageOccupancy(numShipsBySize, shipMaxSize, dim) < BOARD_USAGE_PERCENTAGE)
    {
        n++;
        for (unsigned int i = 0; i < shipMaxSize; i++)
        {
            numShipsBySize[i] = (shipMaxSize - i) * n;
        }
    }

}

void showMenu(bool isGameInitialized)
{
    printf("\n");
    printf("1. Create new game\n");
    printf("2. Load game\n");
    if (isGameInitialized)
    {
        printf("3. Play game\n");
        printf("4. Save game\n");
    }
    printf("5. Highscore\n");
    printf("6. Quit\n");
}

int readMenuEntry(bool isGameInitialized)
{
    if (isGameInitialized)
    {
        return readIntInRange(1, 6, MAX_CHAR_USER_INPUT);
    }
    else
    {
        int validMenuOptions[] = {1, 2, 5, 6};
        return readIntInSet(validMenuOptions, 4, MAX_CHAR_USER_INPUT);
    }
}


int inputBoardDimension()
{
    printf("Enter board dimension\n");
    int dimension = readIntInRange(8, 23, MAX_CHAR_USER_INPUT);
    return dimension;
}

int inputPlayerAmount()
{
    printf("Enter number of players\n");
    return readIntInRange(0, 2, MAX_CHAR_USER_INPUT);
}


void initializePlayer(Player* player, unsigned int dim, unsigned char* numShipsBySize, unsigned char shipMaxSize)
{
    // Reserve memory for the two boards
    player->attackBoard = reserveBoard(dim);
    player->defenseBoard = reserveBoard(dim);

    // Initialize attack board as a not discovered cell
    initializeBoard(player->attackBoard, dim);

    // Put ships on the defense board TODO: add manual selection
    initializeBoardWithShipsAuto(player->defenseBoard, dim, numShipsBySize, shipMaxSize);

    player->lastResult = RESULT_INITIAL;
    player->shot_ships = 0;

}



void initializeGame(Game* game)
{
    // Get size of the board from the user
    game->dim = inputBoardDimension();
    // Get the number of players: 0 for machine vs machine, 1 for human vs machine and 2 for human vs human
    game->num_players = inputPlayerAmount();
    game->players[0].isHuman = false;
    game->players[1].isHuman = false;
    for (unsigned int i = 0; i < game->num_players; i++)
        game->players[i].isHuman = true;

    // Set the number of ships of each size for the game
    game->shipMaxSize = 4;
    game->numShipsBySize = malloc(sizeof(unsigned char) * game->shipMaxSize);
    for (unsigned int i = 0; i < game->shipMaxSize; i++)
    {
        game->numShipsBySize[i] = i + 1;
    }

    // Initialize player instances
    initializePlayer(&game->players[0], game->dim, game->numShipsBySize, game->shipMaxSize);
    initializePlayer(&game->players[1], game->dim, game->numShipsBySize, game->shipMaxSize);
}


void annotateLastShoot(char** attack_board, unsigned int lastResult, Position lastShot, unsigned int dim)
{
    switch(lastResult)
    {
        case RESULT_REPEATED_CELL || RESULT_WATER:
        {
            attack_board[lastShot.x][lastShot.y] = SHOT_WATER;
            break;
        }
        case RESULT_SHOT:
        {
            attack_board[lastShot.x][lastShot.y] = SHOT_SHIP;
            break;
        }
        case RESULT_SHOT_AND_SUNK:
        {
            floodSurroundings(attack_board, lastShot, dim);
            attack_board[lastShot.x][lastShot.y] = SHOT_SHIP;
            break;
        }
        default:
        {
            printf("\nError");
            exit(1);
        }
    }
}


unsigned int getNumberOfBoats(unsigned char* numShipsBySize, unsigned char shipMaxSize)
{
    int accumulator = 0;
    for (int i =0; i < shipMaxSize; i++)
    {
        accumulator += numShipsBySize[i];
    }
    return accumulator;
}


/*
 * For the returning state of playTurn in order to let the user control the flow of algorithm in order to go back to the
 * menu when playing.
 * */
int playTurn(Game* game, unsigned int playerNumber, bool* isPlayerOneTurn)
{
    printf("It's player %i turn. Press any key to continue...\n", playerNumber + 1);
    pauseExecution();

    // If we are human, or we are in machine vs machine show the boards of the player
    if (game->players[playerNumber].isHuman || (!game->players[0].isHuman && !game->players[1].isHuman))
    {
        printf("\n*** ATTACK BOARD PLAYER %i ***", playerNumber + 1);
        showBoard(game->players[playerNumber].attackBoard, game->dim);
        printf("\n*** DEFENSE BOARD PLAYER %i ***", playerNumber + 1 );
        showBoard(game->players[playerNumber].defenseBoard, game->dim);

    }

    // Let the player choose a position to attack if it is a human via user input
    if (game->players[playerNumber].isHuman)
    {
        printf("Player %i: Introduce Row:\t", playerNumber);
        int row = readIntInRange(0, game->dim - 1, MAX_CHAR_USER_INPUT);
        printf("\n");

        printf("Player %i: Introduce Column:\t", playerNumber);
        int col = readIntInRange(0, game->dim - 1, MAX_CHAR_USER_INPUT);
        printf("\n");

        game->players[playerNumber].lastShot.x = row;
        game->players[playerNumber].lastShot.y = col;
    }
    else  // If the player is a machine compute automatically the most optimal position to attack
    {
        game->players[playerNumber].lastShot = computeNextMovement(game->players[playerNumber].attackBoard, game->players[playerNumber].lastShot, game->players[playerNumber].lastResult, game->dim);
    }

    // Perform the shoot on the table of the adversary player
    game->players[playerNumber].lastResult = shoot(game->players[(playerNumber == 0) ? 1 : 0].defenseBoard, game->players[playerNumber].lastShot, game->dim);
    // Annotate the result on the attack board of the current player
    annotateLastShoot(game->players[playerNumber].attackBoard, game->players[playerNumber].lastResult, game->players[playerNumber].lastShot, game->dim);
    printf("The player %i has shot the position %i %i. The result is %i.\n", playerNumber + 1, game->players[playerNumber].lastShot.x, game->players[playerNumber].lastShot.y, game->players[playerNumber].lastResult);

    if (game->players[playerNumber].lastResult == RESULT_SHOT)
    {
        game->players[playerNumber].shot_ships++;
        *isPlayerOneTurn = (playerNumber == 0) ? false : true;
        printf("The player %i has shot a boat. It is his turn again.\n", playerNumber + 1);
    }
    else if (game->players[playerNumber].lastResult == RESULT_SHOT_AND_SUNK)
    {
        game->players[playerNumber].shot_ships++;
        *isPlayerOneTurn = (playerNumber == 0) ? false : true;
        printf("The player %i has sunk a boat. It is his turn again.\n", playerNumber + 1);
    }
    else
    {
        *isPlayerOneTurn = (playerNumber == 0) ? true : false;
    }
    return 0;
}

int play(Game game)
{
    bool isPlayerOneTurn = false;

    showBoard(game.players[0].attackBoard, game.dim);
    showBoard(game.players[0].defenseBoard, game.dim);
    showBoard(game.players[1].attackBoard, game.dim);
    showBoard(game.players[1].defenseBoard, game.dim);
    pauseExecution();

    if (game.num_players == 0)
    {
        isPlayerOneTurn = (rand() % 2 == 1) ? true : false;
    }
    else if (game.num_players == 1)
    {
        isPlayerOneTurn = !throwCoin();
    }
    else if (game.num_players == 2) // num_players == 2 or 3.
    {
        int player_option = rand() % 2;  // Pick user that throws the coin.
        printf("The player %i throws the coin.\n", player_option + 1); // Inform the player who throws the coin.
        if (player_option == 0)
        {
            isPlayerOneTurn = !throwCoin();
        }
        else
        {
            isPlayerOneTurn = throwCoin();
        }
    }


    do
    {
        if (isPlayerOneTurn)
        {
            playTurn(&game, 1, &isPlayerOneTurn);
        }
        else
        {
            playTurn(&game, 0, &isPlayerOneTurn);
        }
    }
    while (game.players[0].shot_ships < computePositionsOccupied(game.numShipsBySize, game.shipMaxSize)
           && game.players[1].shot_ships < computePositionsOccupied(game.numShipsBySize, game.shipMaxSize));

    return 0;
}

void playOne(Game game)
{
    bool isPlayerZeroTurn = false;
    do
    {
        if (isPlayerZeroTurn)
        {
            game.players[1].lastShot = computeNextMovement(game.players[1].attackBoard, game.players[1].lastShot, game.players[1].lastResult, game.dim);
            game.players[1].lastResult = shoot(game.players[0].defenseBoard, game.players[1].lastShot, game.dim);
            annotateLastShoot(game.players[1].attackBoard, game.players[1].lastResult, game.players[1].lastShot, game.dim);
            printf("The machine has shot the position %i %i. The result is %i.\n", game.players[1].lastShot.x, game.players[1].lastShot.y, game.players[1].lastResult);

            if (game.players[1].lastResult == RESULT_SHOT)
            {
                game.players[1].shot_ships++;
                isPlayerZeroTurn = true;
                printf("The machine has shot a boat. It is his turn again.\n");
            }
            else if (game.players[1].lastResult == RESULT_SHOT_AND_SUNK)
            {
                game.players[1].shot_ships++;
                isPlayerZeroTurn = true;
                printf("The machine has sunk a boat. It is his turn again.\n");
            }
            else
            {
                isPlayerZeroTurn = false;
            }
        }
        else  // This is the human player
        {
            showBoard(game.players[0].defenseBoard, game.dim);
            showBoard(game.players[0].attackBoard, game.dim);

            printf("Introduce Row:\t");
            int row = readIntInRange(0, game.dim - 1, MAX_CHAR_USER_INPUT);
            printf("\n");

            printf("Introduce Column:\t");
            int col = readIntInRange(0, game.dim - 1, MAX_CHAR_USER_INPUT);
            printf("\n");

            game.players[0].lastShot.x = row;
            game.players[0].lastShot.y = col;
            game.players[0].lastResult = shoot(game.players[1].defenseBoard, game.players[0].lastShot, game.dim);
            printf("DEBUG human shot\n");
            annotateLastShoot(game.players[0].attackBoard, game.players[0].lastResult, game.players[0].lastShot, game.dim);
            printf("DEBUG anotated result\n");

            if (game.players[0].lastResult == RESULT_SHOT)
            {
                game.players[0].shot_ships++;
                isPlayerZeroTurn = false;
                printf("The human has shot a boat. It is his turn again.\n");

            }
            else if (game.players[0].lastResult == RESULT_SHOT_AND_SUNK)
            {
                game.players[0].shot_ships++;
                isPlayerZeroTurn = false;
                printf("The human has sunk a boat. It is his turn again.\n");
            }
            else
            {
                isPlayerZeroTurn = true;
            }
        }
    }
    while (game.players[0].shot_ships < computePositionsOccupied(game.numShipsBySize, game.shipMaxSize)
            && game.players[1].shot_ships < computePositionsOccupied(game.numShipsBySize, game.shipMaxSize));
}

int columnToIndex(char letter, unsigned int dim)
{
    if (!isCharInRange(letter, 'A', 'A' + dim - 1))
    {
        return ERROR_PARAMETER_OUT_OF_RANGE;
    }
    return letter - 'A';
}

char indexToColumn(int index, unsigned int dim)
{
    if (!isIntInRange(index, 0, dim -1))
    {
        return ERROR_PARAMETER_OUT_OF_RANGE;
    }
    return 'A' + index;
}


int rowToIndex(int row, unsigned int dim)
{
    if (!isIntInRange(row, 1, dim))
    {
        return ERROR_PARAMETER_OUT_OF_RANGE;
    }
    return row - 1;
}

int indexToRow(int index, unsigned int dim)
{
    if (!isIntInRange(index, 0, dim - 1))
    {
        return ERROR_PARAMETER_OUT_OF_RANGE;
    }
    return index + 1;
}

bool throwCoin()
{
    printf("Choose head (H) or tails (T):\t");
    int coinChoiceValue;
    char options[2];
     options[0] = 'H';
     options[1] = 'T';
    char coinChoice = readCharInSet(options, 2);
    if (coinChoice == 'T')
    {
        coinChoiceValue = TAILS;
    }
    else
    {
        coinChoiceValue = HEADS;
    }
    printf("\n");

    if (rand() % 2 == coinChoiceValue)
    {
        printf("You won, first turn is yours.");
        return true;
    }
    else
    {
        printf("You lost, first turn is the enemy.");
        return false;
    }
}

// TODO
// void initializeBoardWithShipsManual(char** board) { }
// void loadRecords(DoubleLinkedList* records) { }
// void saveRecords(DoubleLinkedList records) { }
// int calculateScore(DoubleLinkedList tableResultMoves) { }



