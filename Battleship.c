#include "Battleship.h"

unsigned int DIM = 10;


// PROCEDURE-LIKE (STATIC) FUNCTIONS
void** bulletproof_input(size_t type, void* read_variable)
{
  return 0;
}


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
    {
        return;
    }
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
                    printf("jhvjhv");
                    pause();
                    initializeShip(defense_board, ini, ship_size, orientation);
                    printf("end");
                    pause();
                    floodSurroundings(defense_board, ini);
                    printf("end2");
                    pause();
                    break;
                }
                else  // If it does not fit, annotate another try. If we go through the max tries values, return false
                {
                    number_of_tries++;
                    if (number_of_tries > AUTO_SHIP_PLACEMENT_MAX_TRIES)
                    {
                        return false;
                    }
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

void initializeBoardWithShipsManual(char** board)
{
    return;
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

void floodSurroundings(char** board, Position position)
{
    Position ini = position, end;

    printf("\nlocate%i %i", ini.x, ini.y);
   pause();
    locateShip(board, &ini, &end);

    printf("\nlocateafter%i %i", ini.x, ini.y);
    pause();

    if (ini.x > 0) ini.x--;
    if (ini.y > 0) ini.y--;
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
    // We need an int for our algorithm to be able to do 0-1=-1
    int ini_x = position_ini->x, ini_y = position_ini->y;

    // Move position x end further to the end if a SHIP found
    while (position_end->x < DIM && (board[position_end->x][position_end->y] == SHIP || board[position_end->x][position_end->y] == SHOT_SHIP))
        position_end->x++;
    position_end->x--;

    // Move position y end further to the end if a SHIP found
    while (position_end->y < DIM && (board[position_end->x][position_end->y] == SHIP || board[position_end->x][position_end->y] == SHOT_SHIP))
        (position_end->y)++;
    position_end->y--;

    // Move position x ini further to the beginning if a SHIP found
    while (ini_x >= 0 && (board[ini_x][ini_y] == SHIP || board[ini_x][ini_y] == SHOT_SHIP))
        ini_x--;
    ini_x++;

    // Move position y ini further to the beginning if a SHIP found
    while (ini_y >= 0 && (board[ini_x][ini_y] == SHIP || board[ini_x][ini_y] == SHOT_SHIP))
        ini_y--;
    ini_y++;

    // Return result
    position_ini->x = ini_x;
    position_ini->y = ini_y;
}

bool isSunk(char** board, Position position)
{
    Position end;

    locateShip(board, &position, &end);
    for (unsigned int i = position.x; i <= end.x; i++)
        for (unsigned int j = position.y; j <= end.y; j++)
            if (board[i][j] == SHIP) return false;

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

void computeNextMovement(char** board, Position* position, int result_last_shot)
{
    static unsigned int state = 0;
    if (result_last_shot == RESULT_SHOT_AND_SUNK) state = 0;
    switch (state)
    {
        case 0:
        {

        }
    }
}

int calculateScore(DoubleLinkedList tableResultMoves)
{
    return 0;
}


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

void loadRecords(DoubleLinkedList* records)
{

}

void saveRecords(DoubleLinkedList records)
{

}










int main(int nargs, char* args[])
{
    srand(time(NULL));

    char** board = reserveBoard();

    initializeBoardWithShipsAuto(board);

    showBoard(board);

    printf("%i", naturalLog(9, 10));
    printf("%i", naturalLog(10, 10));

}

