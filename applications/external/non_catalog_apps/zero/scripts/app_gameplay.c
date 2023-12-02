#include "app_gameplay.h"
#include "game.h"
#include "wave/data_structures/list.h"
#include "wave/calc.h"
#include <furi.h>
#include <math.h>

struct AppGameplayState {
    GameState* game;
    int lastActionAt;

    // Controls
    int selectedHandIndex;
    int selectedSuitIndex;
};

GameState* gameplay_get_game(AppGameplayState* gameplay) {
    return gameplay->game;
}

void gameplay_reset(AppGameplayState* gameplay) {
    game_reset(gameplay->game);
    gameplay->lastActionAt = furi_get_tick();
    gameplay->selectedHandIndex = 0;
}

AppGameplayState* gameplay_alloc() {
    AppGameplayState* gameplay = malloc(sizeof(AppGameplayState));
    gameplay->game = game_alloc();
    gameplay_reset(gameplay);

    return gameplay;
}

void gameplay_free(AppGameplayState* gameplay) {
    game_free(gameplay->game);
    free(gameplay);
}

void gameplay_selection_delta(AppGameplayState* gameplay, int delta) {
    int selection = gameplay->selectedHandIndex + delta;
    selection =
        wrap_single(selection, 0, game_get_player_hand_count(gameplay->game, PLAYER_NUMBER) - 1);
    gameplay->selectedHandIndex = selection;
}

int gameplay_selection_get_hand_index(AppGameplayState* gameplay) {
    gameplay_selection_delta(gameplay, 0); // Avoid out of bounds if the hand count changed
    return gameplay->selectedHandIndex;
}

int gameplay_get_selected_card(AppGameplayState* gameplay) {
    GameState* game = gameplay->game;
    int selectedHandIndex = gameplay_selection_get_hand_index(gameplay);

    List* cardsInHand = list_alloc(MAX_HAND_SIZE, sizeof(int));
    game_get_player_hand(game, PLAYER_NUMBER, cardsInHand);
    int cardIndex;
    list_get_at(cardsInHand, selectedHandIndex, &cardIndex);
    list_free(cardsInHand);

    return cardIndex;
}