#include "Battleship.h"

unsigned int DIM = 50;


// PROCEDURE-LIKE (STATIC) FUNCTIONS

/*void* bulletproof_input(size_t type, const int* num_elements, char separator)
{
    char* in = malloc(sizeof(char) * MAX_READ_CHARACTERS);

    bool read = false;

    do {
        printf("\nIntroduce %i elements separated with '%c' of type ", *num_elements, separator);
        switch (type)
        {
            case sizeof(char):
            {
                printf("char");
                break;
            }
            case sizeof(char*):
            {
                printf("char");
                break;
            }
            case sizeof(short int):
            {
                printf("short int");
                break;
            }
            case sizeof(int):
            {
                printf("int");
                break;
            }
            case sizeof(double):
            {
                printf("double (real)");
                break;
            }
            default:
            {
                printf("unknown");
            }
        }
        printf(". End input by pressing Enter: \n> ");

        if (fgets(in, MAX_READ_CHARACTERS, stdin) == NULL)
        {
            printf("\nThe input has not been read. Try again:");
        }
        else
        {
            read = true;
        }
    } while (! read);

    // If we are reading chars it does not make sense to do the separation
    if (sizeof(char) == type)
    // At this point the string is read
    char** result;

}*/


unsigned int naturalLog(unsigned int x, unsigned int base)
{
    unsigned int nlog = 0;
    for (unsigned int i = x; i > 0; i /= base)
        nlog++;
    return nlog - 1;
}

void pause()
{
    char a;
    if (scanf("%c", &a))
        return;
}


// METHOD-LIKE FUNCTIONS FOR THE BATTLESHIP GAME
char** reserveBoard()
{
    // Initialize an array to an array of characters
    char** board = malloc(DIM * sizeof(char*));

    // Initialize each array of characters
    for (unsigned int i = 0; i < DIM; i++)
        board[i] = malloc(DIM * sizeof(char));

    return board;
}

void initializeBoard(char** board)
{
    for (unsigned int i = 0; i < DIM; i++)
        for (unsigned int j = 0; j < DIM; j++)
            board[i][j] = NOT_DISCOVERED_CELL;
}

/**
 * Tries to initialize the board with all the ships and returns true if has been initialized correctly and false if the
 * board is not completely initialized.
 * @param board game board
 * @return true if it is correctly initialized and false if is not completely initialized
 */
bool initializeBoardWithShipsAuto_auxiliar(char** defense_board)
{
    Position ini;
    unsigned int orientation = 0, number_of_tries;

    // Loop through each ship size
    for (unsigned int ship_size = 1; ship_size <= SHIP_MAX_SIZE; ship_size++)
    {
        // Loop through each individual ship
        for (unsigned int ship_counter = 0; ship_counter < NUM_SHIPS_BY_SIZE[ship_size - 1]; ship_counter++)
        {
            number_of_tries = 0;
            // Try to put the current ship on the board
            while (true)
            {
                // Generate random position and orientation to put the current ship
                ini.x = rand() % DIM;
                ini.y = rand() % DIM;
                if (ship_size > 1)
                    orientation = rand() % 2;

                // If it does fit, put it and go to the next ship
                if (doesFit(defense_board, ini, ship_size, orientation))
                {
                    initializeShip(defense_board, ini, ship_size, orientation);
                    floodSurroundings(defense_board, ini);
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

void initializeBoardWithShipsAuto(char** board)
{
    // While the board can not be initialized, keep trying
    do
        initializeBoard(board);
    while (!initializeBoardWithShipsAuto_auxiliar(board));

    // When the board is initialized, substitute not discovered cells with water
    for (unsigned int i = 0; i < DIM; i++)
        for (unsigned int j = 0; j < DIM; j++)
            if (board[i][j] == NOT_DISCOVERED_CELL)
                board[i][j] = WATER;
}

// void initializeBoardWithShipsManual(char** board) { }

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

void floodSurroundings(char** board, Position position)
{
    Position end;

    locateShip(board, &position, &end);

    if (position.x > 0) position.x--;
    if (position.y > 0) position.y--;
    if (end.x < DIM - 1) end.x++;
    if (end.y < DIM - 1) end.y++;

    for (unsigned int i = position.x; i <= end.x; i++)
        for (unsigned int j = position.y; j <= end.y; j++)
            if (board[i][j] != SHOT_SHIP && board[i][j] != SHIP)
                board[i][j] = WATER;
}

void locateShip(char** board, Position* position_ini, Position* position_end)
{
    // Initialize end with the position of reference
    position_end->x = position_ini->x;
    position_end->y = position_ini->y;

    // Move position x end further to the end if a SHIP found
    while (position_end->x + 1 < DIM && (board[position_end->x + 1][position_end->y] == SHIP || board[position_end->x + 1][position_end->y] == SHOT_SHIP))
        position_end->x++;

    // Move position y end further to the end if a SHIP found
    while (position_end->y + 1 < DIM && (board[position_end->x][position_end->y + 1] == SHIP || board[position_end->x][position_end->y + 1] == SHOT_SHIP))
        position_end->y++;

    // Move position x ini further to the beginning if a SHIP found
    while (position_ini->x > 0 && (board[position_ini->x - 1][position_ini->y] == SHIP || board[position_ini->x - 1][position_ini->y] == SHOT_SHIP))
        position_ini->x--;

    // Move position y ini further to the beginning if a SHIP found
    while (position_ini->y > 0 && (board[position_ini->x][position_ini->y - 1] == SHIP || board[position_ini->x][position_ini->y - 1] == SHOT_SHIP))
        position_ini->y--;
}

unsigned int detectOrientation(char** board, Position shipPosition)
{
    // First detect any other ship position in the surroundings
    if (shipPosition.x > 0 && (board[shipPosition.x - 1][shipPosition.y] == SHOT_SHIP || board[shipPosition.x - 1][shipPosition.y] == SHIP))
        return 2;
    else if (shipPosition.y > 0 && (board[shipPosition.x][shipPosition.y - 1] == SHOT_SHIP || board[shipPosition.x][shipPosition.y - 1] == SHIP))
        return 1;
    else if (shipPosition.x < (DIM - 1) && (board[shipPosition.x + 1][shipPosition.y] == SHOT_SHIP || board[shipPosition.x + 1][shipPosition.y] == SHIP))
        return 2;
    else if (shipPosition.y < (DIM - 1) && (board[shipPosition.x][shipPosition.y + 1] == SHOT_SHIP || board[shipPosition.x][shipPosition.y + 1] == SHIP))
        return 1;

    /*
     * Then we check the cases where there is water, shot water or board limits on both sides, assuming that the ship
     * has just one point discovered, but it is longer.
     */
    if ((shipPosition.x == 0 || board[shipPosition.x - 1][shipPosition.y] != NOT_DISCOVERED_CELL) && (shipPosition.x == (DIM - 1) || board[shipPosition.x + 1][shipPosition.y] != NOT_DISCOVERED_CELL))
        return 1;
    else if ((shipPosition.y == 0 || board[shipPosition.x][shipPosition.y - 1] != NOT_DISCOVERED_CELL) && (shipPosition.y == (DIM - 1) || board[shipPosition.x][shipPosition.y + 1] != NOT_DISCOVERED_CELL))
        return 2;
    return 0;
}

bool isSunk(char** board, Position position)
{
    Position end;

    locateShip(board, &position, &end);

    if (position.x > 0)
        position.x--;
    if (end.x < DIM - 1)
        end.x++;
    if (position.y > 0)
        position.y--;
    if (end.y < DIM - 1)
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

bool doesFit(char** defense_board, Position ini, unsigned int ship_size, bool orientation)
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
    if (end.x >= DIM || end.y >= DIM)
        return false;

    // If there are any occupied cells in the limits it does not fit
    for (unsigned int i = ini.x; i <= end.x; i++)
        for (unsigned int j = ini.y; j <= end.y; j++)
            if (defense_board[i][j] != NOT_DISCOVERED_CELL)
                return false;

    return true;
}

unsigned int shoot(char** board, Position position)
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
            if (isSunk(board, position))
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

void computeNextMovement_auxiliarDetectState(char** attack_board, Position iniCurrentShipPosition, unsigned int* state)
{
    Position endCurrentShipPosition;
    // Now we need to obtain info about orientation
    unsigned int orientation = detectOrientation(attack_board, iniCurrentShipPosition);
    printf("\nDetected orientation %u", orientation);  //TODO

    if (! orientation)
    {
        *state = STATE_DESTROY;
    }
    else if (orientation == 1)
    {
        locateShip(attack_board, &iniCurrentShipPosition, &endCurrentShipPosition);
        if (endCurrentShipPosition.y == DIM - 1 || attack_board[endCurrentShipPosition.x][endCurrentShipPosition.y + 1] != NOT_DISCOVERED_CELL)
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

        locateShip(attack_board, &iniCurrentShipPosition, &endCurrentShipPosition);
        if (endCurrentShipPosition.x == DIM - 1 || attack_board[endCurrentShipPosition.x + 1][endCurrentShipPosition.y] != NOT_DISCOVERED_CELL)
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

void generateSurroundingPositions(Position iniPosition, Position* surroundingPositions, unsigned int* surroundingPositionsSize)
{
    unsigned int surroundingCandidatePositionsLength = 0;

    // Check directions starting from iniPosition
    if (iniPosition.x < DIM - 1)
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
    if (iniPosition.y < DIM - 1)
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



Position computeNextMovement(char** attack_board, Position lastShotPosition, unsigned int result_last_shot)
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
        generateSurroundingPositions(lastShotPosition, surroundingPositions, &surroundingCandidatePositionsLength);

        // We need to check the four surrounding cells for a shot but not sunk ship
        unsigned int i = 0;
        while ((i < surroundingCandidatePositionsLength) && ((attack_board[surroundingPositions[i].x][surroundingPositions[i].y] != SHOT_SHIP) || (attack_board[surroundingPositions[i].x][surroundingPositions[i].y] == SHOT_SHIP && isSunk(attack_board,  surroundingPositions[i]))))
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
            computeNextMovement_auxiliarDetectState(attack_board, iniCurrentShipPosition, &state);
        }
    }
    else if (result_last_shot == RESULT_SHOT)
    {
        // Detect state using the position of the last shot ship
        computeNextMovement_auxiliarDetectState(attack_board, lastShotPosition, &state);
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
                result.x = rand() % DIM;
                result.y = rand() % DIM;
            }
            while (attack_board[result.x][result.y] != NOT_DISCOVERED_CELL);
            return result;
        }

        // In this state we need to choose randomly between one of the valid surrounding positions
        case STATE_DESTROY:
        {
            // Get surrounding positions from the visible point of the ship. We can reuse variables
            generateSurroundingPositions(iniCurrentShipPosition, surroundingPositions, &surroundingCandidatePositionsLength);

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

// int calculateScore(DoubleLinkedList tableResultMoves) { }


void showBoard(char** board)
{
    // Obtain the number of digits of DIM
    unsigned int DIM_num_digits = naturalLog(DIM, 10) + 1;

    // Begin printing board in new line
	printf("\n");

    // Before printing the columns (A, B, C...) print as many spaces as the number of digits that DIM has + 1 for the
    // separation
	for (unsigned int i = 0; i < DIM_num_digits + 1; i++)
	    printf(" ");

    // Print the columns row, starting from A
	for (unsigned int i = 0; i < DIM; i++)
		printf("%c", 'A' + i);

    // For each row
	for (unsigned int j = 0; j < DIM; j++)
	{
        // Begin in a new line and print the current row j as number
		printf("\n%i", j + 1);
        // Compute how many digits has the current row number
		unsigned int current_row_num_digits = naturalLog(j + 1, 10) + 1;
        // Print as many spaces as needed to reach the num digits of DIM knowing the num digits of the current row num
        for (unsigned int i = 0; i < DIM_num_digits - current_row_num_digits + 1; i++)
            printf(" ");

        // After the spaces print the board content
		for (unsigned int i = 0; i < DIM; i++)
			printf("%c", board[i][j]);
	}
    // End with newline
	printf("\n");
}

// void loadRecords(DoubleLinkedList* records) { }

// void saveRecords(DoubleLinkedList records) { }


unsigned int computePositionsOccupied()
{
    unsigned int numberPositionsUsed = 0;

    for (unsigned int i = 0; i < SHIP_MAX_SIZE; i++)
    {
        numberPositionsUsed += NUM_SHIPS_BY_SIZE[i] * (i + 1);
    }
    return numberPositionsUsed;
}


float computePercentageOccupancy()
{
    return ((float) computePositionsOccupied() / (float) (DIM * DIM)) * 100;
}


void satisfyUsagePercentage()
{
    printf("%f\n", computePercentageOccupancy());
    unsigned int n = 0;
    float p;
    while (computePercentageOccupancy() < BOARD_USAGE_PERCENTAGE)
    {
        n++;
        p = computePercentageOccupancy();
        printf("\nUsage Percentage: %f, with n %i\n", p, n);
        for (unsigned int i = 0; i < SHIP_MAX_SIZE; i++)
        {
            NUM_SHIPS_BY_SIZE[i] = (SHIP_MAX_SIZE - i) * n;
        }
    }

}


int readInt()
{
    int i;
    int scanfReturn;
    char scanfNewline;
    do
    {
        fflush(stdin);
        scanfReturn = scanf("%i%c", &i, &scanfNewline);

    }
    while (scanfReturn != 2 || scanfNewline != '\n');
    return i;

}








int main()
{
    // printf("%i,%i,%i,%i\n", NUM_SHIPS_BY_SIZE[0], NUM_SHIPS_BY_SIZE[1],NUM_SHIPS_BY_SIZE[2],NUM_SHIPS_BY_SIZE[3]);

    //satisfyUsagePercentage();
    //printf("%i,%i,%i,%i\n", NUM_SHIPS_BY_SIZE[0], NUM_SHIPS_BY_SIZE[1],NUM_SHIPS_BY_SIZE[2],NUM_SHIPS_BY_SIZE[3]);
    printf("%i", readInt());

/*
    srand(time(NULL));

    char** defense_board = reserveBoard();
    char** attack_board = reserveBoard();
    initializeBoard(attack_board);
    initializeBoardWithShipsAuto(defense_board);


    unsigned int shot_ships = 0;
    unsigned int num_ships = 10;
    Position lastShot;
    lastShot.x = 0;
    lastShot.y = 0;
    unsigned int lastResult = RESULT_INITIAL;
    do
    {
        showBoard(defense_board);
        showBoard(attack_board);

        pause();
        
        lastShot = computeNextMovement(attack_board, lastShot, lastResult);
        lastResult = shoot(defense_board, lastShot);
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
                floodSurroundings(attack_board, lastShot);
                attack_board[lastShot.x][lastShot.y] = SHOT_SHIP;
                shot_ships++;
                break;
            }
            default:
            {
                printf("\nError");
                return 1;
            }
        }
    }
    while (shot_ships < num_ships);
    showBoard(defense_board);
    showBoard(attack_board);
    */
}

