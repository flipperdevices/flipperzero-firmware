#pragma once
#include "utilities.h"
#include <stdbool.h>
#include <assert.h>

enum game_state
{
    preloading = 0,
    mainMenu,
    inGame,
    gameOver,
    gameVictory
};
enum shape_names
{
    left = 0,
    top,
    right,
    bottom
};

struct SimonSays
{
    enum game_state gameState;
    int highScore;

    int currentScore;
    int numberOfMillisecondsBeforeShapeDisappears;

    enum shape_names simonMoves[1000];
    int playerMoveIndex;
};

struct SimonSays currentGame;

void resetGame()
{
    currentGame = (struct SimonSays){
        .gameState = preloading,
        .currentScore = 0,
        .playerMoveIndex = 0,
        .numberOfMillisecondsBeforeShapeDisappears = 500,
        .simonMoves[0] = getRandomIntInRange(0, 3)};
}

void preLoadGame()
{
    // TODO: load game stuff
    currentGame.highScore = 0; // TODO: fetch this from storage

    setCurrentGameState(mainMenu);
}

void startGame()
{
    assert(currentGame.gameState == mainMenu);
    setCurrentGameState(inGame);
}

void setCurrentGameState(enum game_state gameState)
{
    currentGame.gameState = gameState;
}

enum game_state getCurrentGameState()
{
    return currentGame.gameState;
}

void addNewSimonMove(int addAtIndex)
{
    currentGame.simonMoves[addAtIndex] = getRandomIntInRange(0, 3);
}

void startNewRound()
{
    addNewSimonMove(currentGame.currentScore);
    currentGame.playerMoveIndex = 0;
}

void onPlayerAnsweredCorrect()
{
    currentGame.playerMoveIndex++;
}

void onPlayerAnsweredWrong()
{
    setCurrentGameState(gameOver);
}

bool isRoundComplete()
{
    return currentGame.playerMoveIndex == currentGame.currentScore;
}

enum shape_names getCurrentSimonMove()
{
    return currentGame.simonMoves[currentGame.playerMoveIndex];
}

void onPlayerSelectedShapeCallback(enum shape_names shape)
{
    if (shape == getCurrentSimonMove())
    {
        onPlayerAnsweredCorrect();
    }
    else
    {
        onPlayerAnsweredWrong();
    }
}