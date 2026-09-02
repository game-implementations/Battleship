#include "libbattleship/libbattleship.h"
#include "libplatform/platform.h"

int main()
{
    // Bring up the host platform (console, input, filesystem) before any I/O.
    platform_init();

    /*
     * Initialize random seed from current time to obtain a different sequence of random number generation when calling
     * the rand() function. Set to a hardcoded number for reproducibility.
     */
    srand(time(NULL));

    DoubleLinkedList* records = initializeRecords();
    Game game;
    game.isGameInitialized = false;
    
    while (platform_should_run())
    {
        showMenu(game.isGameInitialized);
        platform_frame_end();
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
                int result = play(&game);

                if (result != PAUSED_GAME)
                {
                    printf("CONGRATULATIONS to Player %i your WON!\n", result + 1);
                    printf("Game Dim:\t%i\nTotal Shots By Player:\t%i\nScore:\t%i\n", game.dim, game.players[result].totalShots, game.players[result].score);
                    float score = ((float) 100 * ((float) game.dim / (float) game.players[result].totalShots) * (float) game.players[result].score);
                    printf("YOUR SCORE IS:\t%f\n", score);
                    game.isGameInitialized = false;
                }
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
                platform_frame_end();
                platform_shutdown();
                exit(0);
                break;
            }
            default:
            {
                platform_shutdown();
                exit(1);
                break;
            }
        }
    }

    // Reached only if the host asked the app to stop (console targets).
    platform_shutdown();
    return 0;
}