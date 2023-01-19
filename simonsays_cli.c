#include <stdio.h>
#include <assert.h>
#include "simonsays_controller.h"
#include "utilities.h"

int main()
{
    resetGame();

    printf("Loading game...\n");
    // TODO: LOADING SCREEN HERE
    preLoadGame();

    printf("Welcome to Simon Says!\nPress enter to start the game");
    // TODO: MAIN MENU HERE
    getchar();

    startGame();

    while (getCurrentGameState() == inGame)
    {

        printf("New Round!\n");
        currentGame.currentScore++;
        startNewRound();

        enum shape_names currentSimonMove = getCurrentSimonMove();
        printf("Simon Says.. Score(%d)\n", currentGame.currentScore - 1);
        for (int index = 0; index < currentGame.currentScore; index++)
        {
            // TODO: TIMED SEQUENCE HERE
            printf("%d\n", currentGame.simonMoves[index]);
        }

        printf("Your turn:\n");

        while (!isRoundComplete())
        {
            char userInput = getchar();
            if (userInput == '\n')
                continue; // getchar registers enter as an input..
            int playerAction = atoi(&userInput);

            onPlayerSelectedShapeCallback(playerAction);

            // TODO: INTERACT WITH USER INPUTS HERE

            if (getCurrentGameState() != inGame)
            {
                break;
            }
        }
    }

    if (getCurrentGameState() == gameOver)
    {
        // TODO: GAMEOVER SCREEN HERE
        printf("Game over!");
    }
    if (getCurrentGameState() == gameVictory)
    {
        // TODO: VICTORY SCREEN HERE
        printf("You've beat Simon!");
    }

    return 0;
}