#pragma once
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_PLAYERS 4
#define NUMBER_OF_CARDS 108
#define MAX_HAND_SIZE 30
#define NUMBER_OF_STARTING_CARDS 7
#define PLAYER_NUMBER 1

typedef struct List List;

typedef enum ActionType {
    ActionType_None,
    ActionType_Skip,
    ActionType_Invert,
    ActionType_Plus2,
    ActionType_ChangeSuit,
    ActionType_ChangeSuitPlus4,
} ActionType;

typedef enum CardSuit {
    CardSuit_None,
    CardSuit_Spades,
    CardSuit_Hearts,
    CardSuit_Diamonds,
    CardSuit_Clubs,
} CardSuit;

typedef struct Card {
    CardSuit suit;
    int number;
    ActionType action;
} Card;

extern Card AllCardsData[NUMBER_OF_CARDS];
extern const int CARD_NONE;

typedef struct GameState GameState;

GameState* game_alloc();
void game_free(GameState* state);
void game_reset(GameState* state);

int game_get_winner(GameState* state);
void game_update_winner(GameState* state);

void game_set_card_to_play(GameState* state, int cardIndex, int forcedSuit);
int game_get_card_to_play(GameState* state);
CardSuit game_get_forced_suit_to_play(GameState* state);

bool game_is_card_valid_to_play(GameState* state, int cardIndex);
void game_apply_card_to_play(GameState* state);

int game_step_draw_card(GameState* state, int playerNumber);
void game_step_play_card(GameState* state, int cardIndex);
void game_step_next_player(GameState* state);
void game_step_reverse(GameState* state);
void game_step_set_suit(GameState* state, int suit);

int game_get_player_turn(GameState* state);
int game_get_next_player(GameState* state);
int game_calculate_next_player(GameState* state, int playerIndex);
int game_get_top_card(GameState* state);
int game_get_card_location(GameState* state, int index);
CardSuit game_get_forced_suit(GameState* state);
void game_get_player_hand(GameState* state, int playerIndex, List* result);
int game_get_player_hand_count(GameState* state, int playerIndex);
void game_get_player_hand_valid_cards(GameState* state, int playerIndex, List* result);
int game_get_player_hand_valid_cards_count(GameState* state, int playerIndex);
int game_get_direction(GameState* state);