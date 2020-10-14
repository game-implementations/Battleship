#include "Battleship.h"
unsigned int DIM = 100;


/**
 * Prints the board given by parameter.
 * @param board
 */
void showBoard(char* board)
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
			printf("%c", board[j * DIM + i]);
		}
	}
	printf("\n");
}


/**
 * Reserves memory space for a board of the given dimensions, initializes it with NOT_DISCOVERED_CELL and returns it.
 * @return
 */
char* startAttackBoard()
{
    char* attack_board = malloc(DIM * DIM * sizeof(char));
    for (unsigned int i = 0; i < DIM * DIM; i++)
    {
        attack_board[i] = NOT_DISCOVERED_CELL;
    }
    return attack_board;
}

unsigned int shoot(char* board, unsigned int x, unsigned int y)
{
    switch (board[DIM * y + x])
    {
        case SHOT_WATER:
        {
            return 0;
        }
            break;
        case WATER:
        {
            board[DIM * y + x] = SHOT_WATER;
            return 1;
        }
        break;
        case SHIP:
        {
            board[DIM * y + x] = SHOT_SHIP;
            // Detect if ship is sunk and return consequently
        }
        break;
    }
    return 0; // by now to staisfy the compiler
}

int main(int nargs, char* args[])
{
	showBoard(startAttackBoard());
}

