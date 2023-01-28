#include "ttt_multi_game.h"

void ttt_multi_game_reset(TttMultiGame* game) {
    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            game->board[i][j] = TttMultiGamePlayerNone;
        }
    }
}

TttMultiGame* ttt_multi_game_alloc() {
    TttMultiGame* game = malloc(sizeof(TttMultiGame));
    ttt_multi_game_reset(game);
    return game;
}

void ttt_multi_game_free(TttMultiGame* game) {
    free(game);
}
