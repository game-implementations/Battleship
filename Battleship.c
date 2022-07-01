#include "Battleship.h"

unsigned int DIM = 10;

unsigned int naturalLog10(unsigned int x, unsigned int base)
{
    unsigned int nlog10 = 0;
    for (unsigned int i = x; i > 0; i /= base)
        nlog10++;
    return nlog10;
}

void pause()
{
    char a;
    if (scanf("%c", &a))
    {
        return;
    }
}

void showBoard(char** board)
{

    unsigned int DIM_num_digits = 0;
    for (unsigned int j = DIM; j > 0; j /= 10)
    {
        DIM_num_digits++;
    }

	printf("\n ");
	for (unsigned int i = 0; i < DIM_num_digits; i++)
    {
	    printf(" ");
    }
	for (unsigned int i = 0; i < DIM; i++)
	{
		printf("%c", 'A' + i);
	}
	printf("\n");
	for (unsigned int j = 0; j < DIM; j++)
	{	
		printf("\n%i", j + 1);
		unsigned int current_row_num_digits = 0;
        for (unsigned int i = j + 1; i > 0; i /= 10)
        {
            current_row_num_digits++;
        }
        for (unsigned int i = 0; i < DIM_num_digits - current_row_num_digits + 1; i++)
        {
            printf(" ");
        }
		for (unsigned int i = 0; i < DIM; i++)
		{
			printf("%c", board[i][j]);
		}
	}
	printf("\n");
}

void locateShip(char** board, unsigned int* x_ini, unsigned int* y_ini, unsigned int* x_end, unsigned int* y_end)
{
    *x_end = *x_ini;
    *y_end = *y_ini;
    int x_init = *x_ini, y_init = *y_ini;

    while (*x_end < DIM && (board[*x_end][*y_end] == SHIP || board[*x_end][*y_end] == SHOT_SHIP))
        (*x_end)++;
    (*x_end)--;

    while (*y_end < DIM && (board[*x_end][*y_end] == SHIP || board[*x_end][*y_end] == SHOT_SHIP))
        (*y_end)++;
    (*y_end)--;

    while (x_init >= 0 && (board[x_init][*y_ini] == SHIP || board[x_init][*y_ini] == SHOT_SHIP))
        x_init--;
    x_init++;

    while (y_init >= 0 && (board[*x_ini][y_init] == SHIP || board[*x_ini][y_init] == SHOT_SHIP))
        y_init--;
    y_init++;


    *x_ini = x_init;
    *y_ini = y_init;
}

void floodSurroundings(char** board, unsigned int x, unsigned int y)
{
    unsigned int x_ini = x, y_ini = y, x_end, y_end;

    locateShip(board, &x_ini, &y_ini, &x_end, &y_end);

    if (x_ini > 0) x_ini--;
    if (y_ini > 0) y_ini--;
    if (x_end < DIM - 1) x_end++;
    if (y_end < DIM - 1) y_end++;

    for (unsigned int i = x_ini; i <= x_end; i++)
    {
        for (unsigned int j = y_ini; j <= y_end; j++)
        {
            if (board[i][j] != SHOT_SHIP && board[i][j] != SHIP) board[i][j] = WATER;
        }
    }
    //printf("ship in place\n");  pause();  // TODO
}

unsigned int shoot(char** board, unsigned int x, unsigned int y)
{
    switch (board[x][y])
    {
        case SHOT_WATER:
        {
            return 0;
        }
        case WATER:
        {
            board[x][y] = SHOT_WATER;
            return 1;
        }
        case SHIP:
        {
            board[x][y] = SHOT_SHIP;
            if (isSunk(board, x, y))
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

bool isSunk(char** board, unsigned int x, unsigned int y)
{
    unsigned int x_ini = x, y_ini = y, x_end, y_end;
    locateShip(board, &x_ini, &y_ini, &x_end, &y_end);
    for (unsigned int i = x_ini; i <= x_end; i++)
    {
        for (unsigned int j = y_ini; j <= y_end; j++)
        {
            if (board[i][j] == SHIP) return false;
        }
    }
    return true;
}

bool doesFit(char** defense_board, unsigned int x_ini, unsigned int y_ini, unsigned int* x_end, unsigned int* y_end, unsigned int ship_size, bool orientation)
{
    if (ship_size > 1)
    {
        if (orientation)
        {
            (*x_end) = x_ini + ship_size - 1;
            (*y_end) = y_ini;
        }
        else
        {
            (*y_end) = y_ini + ship_size - 1;
            (*x_end) = x_ini;
        }
    }
    else
    {
        (*x_end) = x_ini;
        (*y_end) = y_ini;
    }

    // If we go out of bounds of the board when calculating the limits
    if (*x_end >= DIM || *y_end >= DIM) return false;

    for (unsigned int i = x_ini; i <= (*x_end); i++)
    {
        for (unsigned int j = y_ini; j <= (*y_end); j++)
        {
            //printf("%i %i\n", i, j);  TODO
            if (defense_board[i][j] != NOT_DISCOVERED_CELL) return false;
        }
    }
    return true;
}


void initializeShip(char** defense_board, unsigned int x_ini, unsigned int y_ini, unsigned int x_end, unsigned int y_end)
{
    for (unsigned int i = x_ini; i <= x_end; i++)
    {
        for (unsigned int j = y_ini; j <= y_end; j++)
        {
            defense_board[i][j] = SHIP;
        }
    }
}

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

bool initializeBoardWithShipsAuto_auxiliar(char** defense_board)
{
    unsigned int x_ini, y_ini, x_end, y_end, orientation = 0, number_of_tries;

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
                x_ini = rand() % DIM;
                y_ini = rand() % DIM;
                if (ship_size > 1)
                {
                    orientation = rand() % 2;
                }
                // If it does fit, put it and go to the next ship
                if (doesFit(defense_board, x_ini, y_ini, &x_end, &y_end, ship_size, orientation))
                {
                    initializeShip(defense_board, x_ini, y_ini, x_end, y_end);

                    // printf("\n\nINIT: ship init %u %u %u %u ", x_ini, x_end, y_ini, y_end); // TODO
                    // showBoard(defense_board);  pause(); // TODO

                    floodSurroundings(defense_board, x_ini, y_ini);
                    break;
                }
                else  // If it does not fit, annotate another try. If we go through the max tries values, return false
                {
                    number_of_tries++;
                    if (number_of_tries > 100000)
                    {
                        return false;
                    }
                }
            }

        }
    }
    return true;
}


void computeNextMovement(char** board, unsigned int* x, unsigned int* y, int result_last_shot)
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


int main(int nargs, char* args[])
{
    srand(time(NULL));

    char** board = reserveBoard();

    initializeBoardWithShipsAuto(board);

    showBoard(board);

}

