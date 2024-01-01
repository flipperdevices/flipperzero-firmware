#include "game.h"
#include "utils.h"
#include "move.h"

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
    game->main_menu_pos = 0;
    game->main_menu_mode = NEW_GAME;
    game->hasContinue = false;

    game->state = INTRO;
    game->gameOverReason = NOT_GAME_OVER;

    game->move.frameNo = 0;

    return game;
}

//-----------------------------------------------------------------------------

void free_game_state(Game* game) {
    view_port_free(game->view_port);
    furi_mutex_free(game->mutex);
    free_level_data(game->levelData);
    free_level_set(game->levelSet);
    free_stats(game->stats);
    free(game);
}

//-----------------------------------------------------------------------------

GameOver is_game_over(PlayGround* mv, Stats* stats) {
    uint8_t sumMov = 0;
    uint8_t sum = 0;
    uint8_t x, y;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats->ofBrick[i];
    }
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            sumMov += (*mv)[y][x];
        }
    }
    if((sum > 0) && (sumMov == 0)) {
        return CANNOT_MOVE;
    }
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        if(stats->ofBrick[i] == 1) return BRICKS_LEFT;
    }
    return NOT_GAME_OVER;
}

//-----------------------------------------------------------------------------

bool is_level_finished(Stats* stats) {
    uint8_t sum = 0;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats->ofBrick[i];
    }
    return (sum == 0);
}

//-----------------------------------------------------------------------------

Neighbors find_neighbors(PlayGround* pg, uint8_t x, uint8_t y) {
    Neighbors ne;

    ne.u = (y > 0) ? (*pg)[y - 1][x] : EMPTY_TILE;
    ne.l = (x > 0) ? (*pg)[y][x - 1] : EMPTY_TILE;

    ne.d = (y < SIZE_Y - 1) ? (*pg)[y + 1][x] : EMPTY_TILE;
    ne.r = (x < SIZE_X - 1) ? (*pg)[y][x + 1] : EMPTY_TILE;

    ne.dl = ((y < SIZE_Y - 1) && (x > 0)) ? (*pg)[y + 1][x - 1] : EMPTY_TILE;
    ne.ur = ((y > 0) && (x < SIZE_X - 1)) ? (*pg)[y - 1][x + 1] : EMPTY_TILE;

    ne.ul = ((y > 0) && (x > 0)) ? (*pg)[y - 1][x - 1] : EMPTY_TILE;
    ne.dr = ((x < SIZE_X - 1) && (y < SIZE_Y - 1)) ? (*pg)[y + 1][x + 1] : EMPTY_TILE;

    return ne;
}

//-----------------------------------------------------------------------------

void initial_load_game(Game* game) {
    furi_string_set(game->levelSet->file, APP_ASSETS_PATH("levels/01 Classic Levels.vxl"));
    furi_string_set(game->levelSet->title, "01 Classic Levels");
    Storage* storage = furi_record_open(RECORD_STORAGE);
    load_level_set(storage, furi_string_get_cstr(game->levelSet->file), game->levelSet);
    furi_record_close(RECORD_STORAGE);
    randomize_bg(&game->bg);
}

//-----------------------------------------------------------------------------

void start_game_at_level(Game* game, uint8_t levelNo) {
    game->current_level = levelNo;
    refresh_level(game);
}

//-----------------------------------------------------------------------------

void refresh_level(Game* g) {
    clear_board(&g->board_curr);
    clear_board(&g->board_undo);
    clear_board(&g->board_ani);

    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(load_level(
           storage, furi_string_get_cstr(g->levelSet->file), g->current_level, g->levelData)) {
        parse_level_notation(furi_string_get_cstr(g->levelData->board), &g->board_curr);
    }
    // Close storage
    furi_record_close(RECORD_STORAGE);

    map_movability(&g->board_curr, &g->movables);
    update_board_stats(&g->board_curr, g->stats);
    g->current_movable = find_movable(&g->movables);
    g->undo_movable = MOVABLE_NOT_FOUND;
    g->gameMoves = 0;
    g->state = SELECT_BRICK;
}

//-----------------------------------------------------------------------------

void click_selected(Game* game) {
    const uint8_t dir = movable_dir(&game->movables, game->current_movable);
    switch(dir) {
    case MOVABLE_LEFT:
    case MOVABLE_RIGHT:
        start_move(game, dir);
        break;
    case MOVABLE_BOTH:
        game->state = SELECT_DIRECTION;
        break;
    default:
        break;
    }
}

//-----------------------------------------------------------------------------

void start_gravity(Game* g) {
    uint8_t x, y;
    bool change = false;

    clear_board(&g->board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = (SIZE_Y - 2); y > 0; y--) {
        for(x = (SIZE_X - 1); x > 0; x--) {
            if((is_block(g->board_curr[y][x])) && (g->board_curr[y + 1][x] == EMPTY_TILE)) {
                change = true;
                g->board_ani[y][x] = 1;
            }
        }
    }

    if(change) {
        g->move.frameNo = 0;
        g->move.delay = 5;
        g->state = MOVE_GRAVITY;
    } else {
        g->state = SELECT_BRICK;
        start_explosion(g);
    }
}

//-----------------------------------------------------------------------------

void stop_gravity(Game* g) {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(g->board_ani[y][x] == 1) {
                g->board_curr[y + 1][x] = g->board_curr[y][x];
                g->board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void start_explosion(Game* g) {
    uint8_t x, y;
    bool change = false;

    clear_board(&g->board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(is_block(g->board_curr[y][x])) {
                if(((y > 0) && (g->board_curr[y][x] == g->board_curr[y - 1][x])) ||
                   ((x > 0) && (g->board_curr[y][x] == g->board_curr[y][x - 1])) ||
                   ((y < SIZE_Y - 1) && (g->board_curr[y][x] == g->board_curr[y + 1][x])) ||
                   ((x < SIZE_X - 1) && (g->board_curr[y][x] == g->board_curr[y][x + 1]))) {
                    change = true;
                    g->board_ani[y][x] = 1;
                }
            }
        }
    }

    if(change) {
        g->move.frameNo = 0;
        g->move.delay = 12;
        g->state = EXPLODE;
    } else {
        g->state = SELECT_BRICK;
        movement_stoped(g);
    }
}

//-----------------------------------------------------------------------------

void stop_explosion(Game* g) {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(g->board_ani[y][x] == 1) {
                g->board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void start_move(Game* g, uint8_t direction) {
    g->undo_movable = g->current_movable;
    copy_level(g->board_undo, g->board_curr);
    g->gameMoves++;
    g->move.dir = direction;
    g->move.x = coord_x(g->current_movable);
    g->move.y = coord_y(g->current_movable);
    g->move.frameNo = 0;
    g->next_movable = coord_from((g->move.x + ((direction == MOVABLE_LEFT) ? -1 : 1)), g->move.y);
    g->state = MOVE_SIDES;
}

//-----------------------------------------------------------------------------

void stop_move(Game* g) {
    uint8_t deltaX = ((g->move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;
    uint8_t tile = g->board_curr[g->move.y][g->move.x];

    g->board_curr[g->move.y][g->move.x] = EMPTY_TILE;
    g->board_curr[g->move.y][cap_x(g->move.x + deltaX)] = tile;

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void movement_stoped(Game* g) {
    map_movability(&g->board_curr, &g->movables);
    update_board_stats(&g->board_curr, g->stats);
    g->current_movable = g->next_movable;
    g->next_movable = MOVABLE_NOT_FOUND;
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        find_movable_down(&g->movables, &g->current_movable);
    }
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        find_movable_right(&g->movables, &g->current_movable);
    }
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        g->current_movable = MOVABLE_NOT_FOUND;
    }

    g->gameOverReason = is_game_over(&g->movables, g->stats);

    if(g->gameOverReason > NOT_GAME_OVER) {
        g->state = GAME_OVER;
    } else if(is_level_finished(g->stats)) {
        g->state = LEVEL_FINISHED;
    } else {
        g->state = SELECT_BRICK;
    }
}

//-----------------------------------------------------------------------------

bool undo(Game* g) {
    if(g->undo_movable != MOVABLE_NOT_FOUND) {
        g->current_movable = g->undo_movable;
        g->undo_movable = MOVABLE_NOT_FOUND;
        copy_level(g->board_curr, g->board_undo);
        map_movability(&g->board_curr, &g->movables);
        update_board_stats(&g->board_curr, g->stats);
        g->gameMoves--;
        g->state = SELECT_BRICK;
        return true;
    } else {
        g->state = SELECT_BRICK;
        return false;
    }
}