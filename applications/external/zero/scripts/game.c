#include "game.h"
#include "wave/data_structures/queue.h"
#include "wave/data_structures/list.h"
#include "wave/exception_manager.h"
#include <gui/gui.h>
#include <stdlib.h>
#include <math.h>

Card AllCardsData[NUMBER_OF_CARDS];
const int CARD_NONE = -1;

struct GameState {
    int cardLocation[NUMBER_OF_CARDS]; // 0 = deck, -1 = discard, 1+ = player index
    int playerTurn;
    int direction;
    int topCard;
    CardSuit forcedSuit;

    int cardToPlay;
    CardSuit forcedSuitToPlay;

    int winner;

    Queue* shuffleQueue;
};

void fill_shuffle_queue(GameState* state) {
    int array[NUMBER_OF_CARDS];
    for(int i = 0; i < NUMBER_OF_CARDS; i++) array[i] = i;

    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        int j = rand() % NUMBER_OF_CARDS;
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }

    for(int i = 0; i < NUMBER_OF_CARDS; i++) queue_enqueue(state->shuffleQueue, &array[i]);
}

int game_step_draw_card(GameState* state, int playerNumber) {
    if(game_get_player_hand_count(state, 0) == 0) {
        for(int i = 0; i < NUMBER_OF_CARDS; i++)
            if(state->cardLocation[i] == -1) state->cardLocation[i] = 0;
    }

    int cardIndex;
    do {
        if(queue_count(state->shuffleQueue) == 0) fill_shuffle_queue(state);

        queue_dequeue(state->shuffleQueue, &cardIndex);
    } while(state->cardLocation[cardIndex] != 0);

    state->cardLocation[cardIndex] = playerNumber;
    return cardIndex;
}

bool game_is_draw_four_valid_to_play(GameState* state, int playerIndex) {
    Card topCard = AllCardsData[game_get_top_card(state)];
    CardSuit forbiddenSuit = state->forcedSuit != CardSuit_None ? state->forcedSuit : topCard.suit;

    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        if(state->cardLocation[i] == playerIndex) {
            Card card = AllCardsData[i];
            if(card.suit == forbiddenSuit) return false;
        }
    }
    return true;
}

bool game_is_card_valid_to_play(GameState* state, int cardIndex) {
    Card card = AllCardsData[cardIndex];
    Card topCard = AllCardsData[game_get_top_card(state)];

    // Playing a joker
    if(card.action == ActionType_ChangeSuit) return true;

    if(card.action == ActionType_ChangeSuitPlus4)
        return game_is_draw_four_valid_to_play(state, state->playerTurn);

    // Playing on a joker (forced suit)
    if(state->forcedSuit != CardSuit_None && card.suit == state->forcedSuit) return true;

    // Playing a card of the same suit or number
    if(card.suit == topCard.suit) return true;

    if(card.number != -1 && card.number == topCard.number) return true;

    // Playing an action card of the same action
    if(card.action != ActionType_None && card.action == topCard.action) return true;

    return false;
}

void assert_valid_play(GameState* state, int cardIndex, CardSuit forcedSuit) {
    char message[100];
    Card card = AllCardsData[cardIndex];

    bool isForcingSuit = forcedSuit != CardSuit_None;
    bool requiresForcedSuit = card.action == ActionType_ChangeSuit ||
                              card.action == ActionType_ChangeSuitPlus4;

    if(isForcingSuit != requiresForcedSuit) {
        snprintf(message, 100, "Invalid forced suit: %d on %d", forcedSuit, card.action);
        throw_exception(message);
    }

    if(!game_is_card_valid_to_play(state, cardIndex)) {
        snprintf(message, 100, "Invalid play: %d on %d", cardIndex, state->topCard);
        throw_exception(message);
    }
}

int game_get_winner(GameState* state) {
    return state->winner;
}

void game_update_winner(GameState* state) {
    for(int playerIndex = 1; playerIndex <= NUMBER_OF_PLAYERS; playerIndex++) {
        if(game_get_player_hand_count(state, playerIndex) == 0) {
            state->winner = playerIndex;
            break;
        }
    }
}

void game_set_card_to_play(GameState* state, int cardIndex, int forcedSuit) {
    assert_valid_play(state, cardIndex, forcedSuit);
    state->cardToPlay = cardIndex;
    state->forcedSuitToPlay = forcedSuit;
}

int game_get_card_to_play(GameState* state) {
    return state->cardToPlay;
}

CardSuit game_get_forced_suit_to_play(GameState* state) {
    return state->forcedSuitToPlay;
}

void game_apply_card_to_play(GameState* state) {
    int cardIndex = state->cardToPlay;
    CardSuit forcedSuit = state->forcedSuitToPlay;

    game_step_play_card(state, cardIndex);
    if(game_get_winner(state) != 0) return;

    Card card = AllCardsData[cardIndex];

    if(card.action == ActionType_Invert) game_step_reverse(state);

    if(card.action == ActionType_Skip) game_step_next_player(state);

    if(card.action == ActionType_ChangeSuit) game_step_set_suit(state, forcedSuit);

    if(card.action == ActionType_Plus2) {
        game_step_draw_card(state, game_get_next_player(state));
        game_step_draw_card(state, game_get_next_player(state));
        game_step_next_player(state);
    }

    if(card.action == ActionType_ChangeSuitPlus4) {
        game_step_draw_card(state, game_get_next_player(state));
        game_step_draw_card(state, game_get_next_player(state));
        game_step_draw_card(state, game_get_next_player(state));
        game_step_draw_card(state, game_get_next_player(state));
        game_step_set_suit(state, forcedSuit);
        game_step_next_player(state);
    }

    state->cardToPlay = CARD_NONE;
    state->forcedSuitToPlay = CardSuit_None;

    // Next player
    game_step_next_player(state);
}

void game_step_play_card(GameState* state, int cardIndex) {
    state->cardLocation[cardIndex] = -1;
    state->topCard = cardIndex;
    state->forcedSuit = CardSuit_None;

    game_update_winner(state);
}

void game_step_next_player(GameState* state) {
    state->playerTurn = game_get_next_player(state);
}

void game_step_reverse(GameState* state) {
    state->direction = -state->direction;
}

void game_step_set_suit(GameState* state, int suit) {
    state->forcedSuit = suit;
}

void initialize_cards_data() {
    // Initializes cards data.
    // Order is useful: it will be used when displaying the cards so they are already sorted.

    static bool initialized = false;
    if(initialized) return;
    initialized = true;

    int cardIndex = 0;
    for(int suit = 1; suit <= 4; suit++) {
        for(int number = 0; number < 10; number++) {
            Card card;
            card.suit = suit;
            card.number = number;
            card.action = ActionType_None;

            AllCardsData[cardIndex++] = card;
            if(number != 0) // 0 is only once per suit
                AllCardsData[cardIndex++] = card;
        }
    }

    for(int suit = 1; suit <= 4; suit++) {
        Card card;

        card = (Card){.suit = suit, .number = -1, .action = ActionType_Skip};
        AllCardsData[cardIndex++] = card;
        AllCardsData[cardIndex++] = card;

        card = (Card){.suit = suit, .number = -1, .action = ActionType_Invert};
        AllCardsData[cardIndex++] = card;
        AllCardsData[cardIndex++] = card;

        card = (Card){.suit = suit, .number = -1, .action = ActionType_Plus2};
        AllCardsData[cardIndex++] = card;
        AllCardsData[cardIndex++] = card;
    }

    for(int i = 0; i < 4; i++) {
        Card card = {.suit = CardSuit_None, .number = -1, .action = ActionType_ChangeSuit};
        AllCardsData[cardIndex++] = card;
    }

    for(int i = 0; i < 4; i++) {
        Card card = {.suit = CardSuit_None, .number = -1, .action = ActionType_ChangeSuitPlus4};
        AllCardsData[cardIndex++] = card;
    }
}

GameState* game_alloc() {
    GameState* state = malloc(sizeof(GameState));
    state->shuffleQueue = queue_alloc(NUMBER_OF_CARDS, sizeof(int));
    game_reset(state);
    return state;
}

void game_free(GameState* state) {
    queue_free(state->shuffleQueue);
    free(state);
}

void game_reset_top_card(GameState* state) {
    int cardIndex;
    do {
        cardIndex = rand() % NUMBER_OF_CARDS;
    } while(AllCardsData[cardIndex].number == -1);

    game_step_play_card(state, cardIndex);
}

void game_reset(GameState* state) {
    initialize_cards_data();

    for(int i = 0; i < NUMBER_OF_CARDS; i++) state->cardLocation[i] = 0;

    for(int playerIndex = 1; playerIndex <= NUMBER_OF_PLAYERS; playerIndex++) {
        for(int j = 0; j < NUMBER_OF_STARTING_CARDS; j++) {
            game_step_draw_card(state, playerIndex);
        }
    }

    game_reset_top_card(state);
    state->playerTurn = rand() % NUMBER_OF_PLAYERS + 1;
    state->direction = rand() % 2 == 0 ? 1 : -1;
    state->cardToPlay = CARD_NONE;
    state->forcedSuitToPlay = CardSuit_None;
    state->winner = 0;
}

int game_get_player_turn(GameState* state) {
    return state->playerTurn;
}

int game_get_next_player(GameState* state) {
    return game_calculate_next_player(state, state->playerTurn);
}

int game_calculate_next_player(GameState* state, int playerIndex) {
    return (playerIndex - 1 + state->direction + NUMBER_OF_PLAYERS) % NUMBER_OF_PLAYERS + 1;
}

int game_get_top_card(GameState* state) {
    return state->topCard;
}

int game_get_card_location(GameState* state, int index) {
    return state->cardLocation[index];
}

CardSuit game_get_forced_suit(GameState* state) {
    return state->forcedSuit;
}

void game_get_player_hand(GameState* state, int playerIndex, List* result) {
    list_clear(result);
    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        if(state->cardLocation[i] == playerIndex) list_add(result, &i);
    }
}

int game_get_player_hand_count(GameState* state, int playerIndex) {
    int count = 0;
    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        if(state->cardLocation[i] == playerIndex) count++;
    }
    return count;
}

void game_get_player_hand_valid_cards(GameState* state, int playerIndex, List* result) {
    list_clear(result);
    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        if(state->cardLocation[i] == playerIndex && game_is_card_valid_to_play(state, i))
            list_add(result, &i);
    }
}

int game_get_player_hand_valid_cards_count(GameState* state, int playerIndex) {
    int count = 0;
    for(int i = 0; i < NUMBER_OF_CARDS; i++) {
        if(state->cardLocation[i] == playerIndex && game_is_card_valid_to_play(state, i)) count++;
    }
    return count;
}

int game_get_direction(GameState* state) {
    return state->direction;
}