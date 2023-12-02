#pragma once
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_PLAYERS 4
#define NUMBER_OF_CARDS 108
#define MAX_HAND_SIZE 30
#define NUMBER_OF_STARTING_CARDS 7
#define PLAYER_NUMBER 1

typedef struct List List;

typedef enum ActionType
{
    ActionType_None,
    ActionType_Skip,
    ActionType_Invert,
    ActionType_Plus2,
    ActionType_ChangeSuit,
    ActionType_ChangeSuitPlus4,
} ActionType;

typedef enum CardSuit
{
    CardSuit_None,
    CardSuit_Spades,
    CardSuit_Hearts,
    CardSuit_Diamonds,
    CardSuit_Clubs,
} CardSuit;

typedef struct Card
{
    CardSuit suit;
    int number;
    ActionType action;
} Card;

extern Card AllCardsData[NUMBER_OF_CARDS];
extern const int CARD_NONE;

typedef struct LevelState LevelState;

LevelState* game_alloc();
void game_free(LevelState* state);
void game_reset(LevelState* state);
