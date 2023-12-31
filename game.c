#include "game.h"

Game* alloc_game_state(int* error) {
    *error = 0;
    Game* game = malloc(sizeof(Game));

    game->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game->mutex) {
        FURI_LOG_E("Vexed", "cannot create mutex\r\n");
        free(game);
        *error = 255;
        return NULL;
    }

    game->levelData = alloc_level_data();
    game->levelSet = alloc_level_set();
    game->stats = alloc_stats();

    game->current_level = 0; //4   16  21??? 22  32
    game->gameMoves = 0;

    game->undo_movable = MOVABLE_NOT_FOUND;
    game->current_movable = MOVABLE_NOT_FOUND;
    game->next_movable = MOVABLE_NOT_FOUND;
    game->menu_paused_pos = 0;

    game->state = SELECT_BRICK;
    game->gameOverReason = NOT_GAME_OVER;

    return game;
}

void free_game_state(Game* game) {
    view_port_free(game->view_port);
    furi_mutex_free(game->mutex);
    free_level_data(game->levelData);
    free_level_set(game->levelSet);
    free_stats(game->stats);
    free(game);
}