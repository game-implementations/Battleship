#include "libbattleship/libbattleship.h"

int main()
{
    /*
     * Initialize random seed from current time to obtain a different sequence of random number generation when calling
     * the rand() function. Set to a hardcoded number for reproducibility.
     */
    srand(time(NULL));


    DoubleLinkedList* records = initializeRecords();
    Record recordTest;
    Record recordPaco;
    Record recordMaria;
    Record recordDani;

    char playerName[] = "jose\0";
    recordTest.playerName = malloc(sizeof(char) * MAX_CHAR_USER_INPUT);
    // void* memcpy(void* dest, const void* src, size_t n);
    memcpy((void*) recordTest.playerName, (void *) playerName, 5);

    recordTest.score = 31;
    recordPaco.score = 335;
    recordMaria.score = 333;
    recordDani.score = 3;
    char pacoName[] = "paco\0";
    recordPaco.playerName = malloc(sizeof(char) * MAX_CHAR_USER_INPUT);
    // void* memcpy(void* dest, const void* src, size_t n);
    memcpy((void*) recordPaco.playerName, (void *) pacoName, 5);
    char mariaName[] = "maria\0";
    recordMaria.playerName = malloc(sizeof(char) * MAX_CHAR_USER_INPUT);
    // void* memcpy(void* dest, const void* src, size_t n);
    memcpy((void*) recordMaria.playerName, (void *) mariaName, 5);
    char daniName[] = "dani\0";
    recordDani.playerName = malloc(sizeof(char) * MAX_CHAR_USER_INPUT);
    // void* memcpy(void* dest, const void* src, size_t n);
    memcpy((void*) recordDani.playerName, (void *) daniName, 5);


    addRecord(records, recordTest);
    addRecord(records, recordPaco);
    addRecord(records, recordMaria);
    addRecord(records, recordDani);
    addRecord(records, recordTest);

    showRecords(*records);


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