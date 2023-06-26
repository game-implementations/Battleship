#include "libbattleship/libbattleship.h"

int main()
{
    /*
     * Initialize random seed from current time to obtain a different sequence of random number generation when calling
     * the rand() function. Set to a hardcoded number for reproducibility.
     */
    srand(time(NULL));

    Game game;
    game.isGameInitialized = false;

    while (true)
    {
        showMenu(game.isGameInitialized);
        int menuOption = readMenuEntry(game.isGameInitialized);
        switch (menuOption)
        {
            case 1:
            {
                printf("Creating new game...\n");
                initializeGame(&game);
                game.isGameInitialized = true;
                break;
            }
            case 2:
            {
                printf("Loading game...\n");
                game.isGameInitialized = true;
                break;
            }
            case 3:
            {
                printf("Play game...\n");
                play(game);
                game.isGameInitialized = false;
                break;
            }
            case 4:
            {
                printf("Saving game...\n");
                break;
            }
            case 5:
            {
                printf("Showing Highscore...\n");
                break;
            }
            case 6:
            {
                printf("Leaving game...\n");
                exit(0);
                break;
            }
            default:
            {
                exit(1);
                break;
            }
        }
    }
}