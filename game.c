#include "game.h"
#include "utils.h"
#include "move.h"

Game* alloc_game_state(int* error) {
    *error = 0;
    Game* game = malloc(sizeof(Game));

    game->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(game);
        *error = 255;
        return NULL;
    }

    game->levelData = alloc_level_data();
    game->levelSet = alloc_level_set();
    game->stats = alloc_stats();

    game->currentLevel = 0;
    game->gameMoves = 0;
    game->score = 0;

    game->currentMovableBackup = MOVABLE_NOT_FOUND;
    game->solutionMode = false;
    game->solutionStep = 0;
    game->solutionTotal = 0;

    game->undoMovable = MOVABLE_NOT_FOUND;
    game->currentMovable = MOVABLE_NOT_FOUND;
    game->nextMovable = MOVABLE_NOT_FOUND;
    game->menuPausedPos = 0;

    game->mainMenuBtn = MODE_BTN;
    game->mainMenuMode = NEW_GAME;
    game->mainMenuInfo = false;
    game->hasContinue = false;
    game->selectedSet = furi_string_alloc_set(assetLevels[0]);
    game->selectedLevel = 0;
    game->continueSet = furi_string_alloc_set(assetLevels[0]);
    game->continueLevel = 0;
    game->setPos = 0;
    game->setCount = 1;

    game->state = INTRO;
    game->gameOverReason = NOT_GAME_OVER;

    game->move.frameNo = 0;

    memset(game->parLabel, 0, PAR_LABEL_SIZE);
    game->errorMsg = furi_string_alloc();

    return game;
}

//-----------------------------------------------------------------------------

void load_game_board(Game* g) {
    bool levelLoadable = false;
    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(load_level(storage, g->levelSet->id, g->currentLevel, g->levelData, g->errorMsg)) {
        levelLoadable = parse_level_notation(furi_string_get_cstr(g->levelData->board), &g->board);
    }
    // Close storage

    if(!levelLoadable) {
        handle_ivalid_set(g, storage, g->levelSet->id, g->errorMsg);
    }

    furi_record_close(RECORD_STORAGE);
}

//-----------------------------------------------------------------------------

void free_game_state(Game* game) {
    view_port_free(game->viewPort);
    furi_mutex_free(game->mutex);
    free_level_data(game->levelData);
    free_level_set(game->levelSet);
    free_stats(game->stats);
    furi_string_free(game->selectedSet);
    furi_string_free(game->continueSet);
    furi_string_free(game->errorMsg);
    free_level_list(&game->levelList);
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

void index_set(Game* game) {
    const char* findSetId = furi_string_get_cstr(game->levelSet->id);
    game->setCount = level_count(game);
    game->setPos = 0;
    for(uint8_t i = 0; i < ASSETS_LEVELS_COUNT; i++) {
        if(strcmp(findSetId, assetLevels[i]) == 0) {
            game->setPos = i;
            return;
        }
    }

    if(game->levelList.ids != NULL) {
        for(uint8_t j = 0; j < game->levelList.count; j++) {
            if(strcmp(findSetId, furi_string_get_cstr(game->levelList.ids[j])) == 0) {
                game->setPos = ASSETS_LEVELS_COUNT + j;
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void recalc_score(Game* g) {
    g->score = 0;
    for(uint8_t i = 0; i < g->levelSet->maxLevel; i++) {
        if(g->levelSet->scores[i].moves > 0) {
            g->score += g->levelSet->scores[i].moves - g->levelSet->pars[i];
        }
        if(g->levelSet->scores[i].spoiled) {
            g->score += 5;
        }
    }
}

//-----------------------------------------------------------------------------

void handle_ivalid_set(Game* game, Storage* storage, FuriString* setId, FuriString* errorMsg) {
    mark_set_invalid(storage, setId, errorMsg);
    list_extra_levels(storage, &game->levelList);
    furi_string_set(game->errorMsg, "Invalid level: ");
    furi_string_cat(game->errorMsg, setId);
    furi_string_set(game->selectedSet, assetLevels[0]);
    game->mainMenuMode = CUSTOM;
    game->selectedLevel = 0;
    game->mainMenuBtn = LEVELSET_BTN;
    load_level_set(storage, game->selectedSet, game->levelSet, game->errorMsg);
    game->state = INVALID_PROMPT;
}

//-----------------------------------------------------------------------------

void initial_load_game(Game* game) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    list_extra_levels(storage, &game->levelList);

    game->hasContinue = load_last_level(game->continueSet, &game->continueLevel);

    if(game->hasContinue) {
        furi_string_set(game->selectedSet, game->continueSet);
        game->selectedLevel = game->continueLevel + 1;
        game->mainMenuMode = CONTINUE;
    } else {
        furi_string_set(game->selectedSet, assetLevels[0]);
        game->selectedLevel = 0;
        game->mainMenuMode = NEW_GAME;
    }

    if(!load_level_set(storage, game->selectedSet, game->levelSet, game->errorMsg)) {
        handle_ivalid_set(game, storage, game->selectedSet, game->errorMsg);
    }
    furi_record_close(RECORD_STORAGE);
    index_set(game);
    recalc_score(game);

    if(game->selectedLevel > game->levelSet->maxLevel - 1) {
        game->selectedLevel = game->levelSet->maxLevel - 1;
    }

    randomize_bg(&game->bg);
}

//-----------------------------------------------------------------------------

void new_game(Game* game) {
    forget_continue(game);
    FuriString* setName = furi_string_alloc_set(assetLevels[0]);
    load_gameset_if_needed(game, setName);
    furi_string_free(setName);
    start_game_at_level(game, 0);
}

//-----------------------------------------------------------------------------

void load_gameset_if_needed(Game* game, FuriString* expectedSet) {
    if(furi_string_cmp(expectedSet, game->levelSet->id) != 0) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        if(!load_level_set(storage, expectedSet, game->levelSet, game->errorMsg)) {
            handle_ivalid_set(game, storage, game->selectedSet, game->errorMsg);
        }
        furi_record_close(RECORD_STORAGE);
    }
    index_set(game);
    recalc_score(game);
}

//-----------------------------------------------------------------------------

const char* level_on_pos(Game* game, int pos) {
    if(pos < ASSETS_LEVELS_COUNT) {
        return assetLevels[pos];
    } else {
        int adjPos = pos - ASSETS_LEVELS_COUNT;
        FURI_LOG_D(TAG, "Level for exra %d, %d", pos, adjPos);
        if((game->levelList.ids != NULL) && (adjPos < game->levelList.count)) {
            if(game->levelList.ids[adjPos] != NULL) {
                return furi_string_get_cstr(game->levelList.ids[adjPos]);
            } else {
                return assetLevels[ASSETS_LEVELS_COUNT - 1];
            }
        } else {
            return assetLevels[ASSETS_LEVELS_COUNT - 1];
        }
    }

    return assetLevels[0];
}

//-----------------------------------------------------------------------------

int level_count(Game* game) {
    return ASSETS_LEVELS_COUNT + ((game->levelList.ids != NULL) ? game->levelList.count : 0);
}

//-----------------------------------------------------------------------------

void start_game_at_level(Game* game, uint8_t levelNo) {
    if(levelNo < game->levelSet->maxLevel) {
        game->currentLevel = levelNo;
        refresh_level(game);
    } else {
        game->mainMenuBtn = LEVELSET_BTN;
        game->mainMenuMode = CUSTOM;

        game->setPos = (game->setPos < game->setCount - 1) ? game->setPos + 1 : 0;
        furi_string_set(game->selectedSet, level_on_pos(game, game->setPos));
        load_gameset_if_needed(game, game->selectedSet);
        game->selectedLevel = 0;

        game->state = MAIN_MENU;
    }
}

//-----------------------------------------------------------------------------

void score_for_level(Game* g, uint8_t levelNo, char* buf, size_t max) {
    if(g->levelSet->scores[levelNo].moves == 0) {
        snprintf(buf, max, "???");
    } else {
        if(g->levelSet->scores[levelNo].moves == g->levelSet->pars[levelNo]) {
            snprintf(buf, max, "par");
        } else {
            snprintf(
                buf, max, "%+d", g->levelSet->scores[levelNo].moves - g->levelSet->pars[levelNo]);
        }
    }
}

//-----------------------------------------------------------------------------

void refresh_level(Game* g) {
    clear_board(&g->board);
    clear_board(&g->boardUndo);
    clear_board(&g->toAnimate);

    furi_string_set(g->selectedSet, g->levelSet->id);
    furi_string_set(g->continueSet, g->levelSet->id);

    g->selectedLevel = g->currentLevel;
    load_game_board(g);

    map_movability(&g->board, &g->movables);
    update_board_stats(&g->board, g->stats);
    g->currentMovable = find_movable(&g->movables);
    g->undoMovable = MOVABLE_NOT_FOUND;
    g->gameMoves = 0;
    g->state = SELECT_BRICK;

    memset(g->parLabel, 0, PAR_LABEL_SIZE);
    score_for_level(g, g->selectedLevel, g->parLabel, PAR_LABEL_SIZE);
}

//-----------------------------------------------------------------------------

void level_finished(Game* g) {
    g->hasContinue = true;
    furi_string_set(g->selectedSet, g->levelSet->id);
    furi_string_set(g->continueSet, g->levelSet->id);
    g->continueLevel = g->currentLevel;

    uint16_t moves = (uint16_t)g->gameMoves;
    if((moves < g->levelSet->scores[g->currentLevel].moves) ||
       (g->levelSet->scores[g->currentLevel].moves == 0)) {
        g->levelSet->scores[g->currentLevel].moves = moves;
    }

    save_last_level(g->levelSet->id, g->currentLevel);
    save_set_scores(g->levelSet->id, g->levelSet->scores);
    recalc_score(g);
}

//-----------------------------------------------------------------------------

void forget_continue(Game* game) {
    game->hasContinue = false;
    furi_string_set(game->selectedSet, assetLevels[0]);
    furi_string_set(game->continueSet, assetLevels[0]);
    game->selectedLevel = 0;
    game->continueLevel = 0;
    delete_progress(game->levelSet->scores);
    recalc_score(game);
}

//-----------------------------------------------------------------------------

void click_selected(Game* game) {
    const uint8_t dir = movable_dir(&game->movables, game->currentMovable);
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

    clear_board(&g->toAnimate);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = (SIZE_Y - 2); y > 0; y--) {
        for(x = (SIZE_X - 1); x > 0; x--) {
            if((is_block(g->board[y][x])) && (g->board[y + 1][x] == EMPTY_TILE)) {
                change = true;
                g->toAnimate[y][x] = 1;
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
            if(g->toAnimate[y][x] == 1) {
                g->board[y + 1][x] = g->board[y][x];
                g->board[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void start_explosion(Game* g) {
    uint8_t x, y;
    bool change = false;

    clear_board(&g->toAnimate);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(is_block(g->board[y][x])) {
                if(((y > 0) && (g->board[y][x] == g->board[y - 1][x])) ||
                   ((x > 0) && (g->board[y][x] == g->board[y][x - 1])) ||
                   ((y < SIZE_Y - 1) && (g->board[y][x] == g->board[y + 1][x])) ||
                   ((x < SIZE_X - 1) && (g->board[y][x] == g->board[y][x + 1]))) {
                    change = true;
                    g->toAnimate[y][x] = 1;
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
            if(g->toAnimate[y][x] == 1) {
                g->board[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void start_move(Game* g, uint8_t direction) {
    if(!g->solutionMode) {
        g->undoMovable = g->currentMovable;
        copy_level(g->boardUndo, g->board);
        g->gameMoves++;
    }
    g->move.dir = direction;
    g->move.x = coord_x(g->currentMovable);
    g->move.y = coord_y(g->currentMovable);
    g->move.frameNo = 0;
    if(!g->solutionMode) {
        g->nextMovable =
            coord_from((g->move.x + ((direction == MOVABLE_LEFT) ? -1 : 1)), g->move.y);
    }
    g->state = MOVE_SIDES;
}

//-----------------------------------------------------------------------------

void stop_move(Game* g) {
    uint8_t deltaX = ((g->move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;
    uint8_t tile = g->board[g->move.y][g->move.x];

    g->board[g->move.y][g->move.x] = EMPTY_TILE;
    g->board[g->move.y][cap_x(g->move.x + deltaX)] = tile;

    start_gravity(g);
}

//-----------------------------------------------------------------------------

void movement_stoped(Game* g) {
    if(g->solutionMode) {
        solution_next(g);
    } else {
        map_movability(&g->board, &g->movables);
        update_board_stats(&g->board, g->stats);
        g->currentMovable = g->nextMovable;
        g->nextMovable = MOVABLE_NOT_FOUND;
        if(!is_block(g->board[coord_y(g->currentMovable)][coord_x(g->currentMovable)])) {
            find_movable_down(&g->movables, &g->currentMovable);
        }
        if(!is_block(g->board[coord_y(g->currentMovable)][coord_x(g->currentMovable)])) {
            find_movable_right(&g->movables, &g->currentMovable);
        }
        if(!is_block(g->board[coord_y(g->currentMovable)][coord_x(g->currentMovable)])) {
            g->currentMovable = MOVABLE_NOT_FOUND;
        }

        g->gameOverReason = is_game_over(&g->movables, g->stats);

        if(g->gameOverReason > NOT_GAME_OVER) {
            g->state = GAME_OVER;
        } else if(is_level_finished(g->stats)) {
            g->state = LEVEL_FINISHED;
            level_finished(g);
        } else {
            g->state = SELECT_BRICK;
        }
    }
}

//-----------------------------------------------------------------------------

bool undo(Game* g) {
    if(g->undoMovable != MOVABLE_NOT_FOUND) {
        g->currentMovable = g->undoMovable;
        g->undoMovable = MOVABLE_NOT_FOUND;
        copy_level(g->board, g->boardUndo);
        map_movability(&g->board, &g->movables);
        update_board_stats(&g->board, g->stats);
        g->gameMoves--;
        g->state = SELECT_BRICK;
        return true;
    } else {
        g->state = SELECT_BRICK;
        return false;
    }
}

//-----------------------------------------------------------------------------

uint8_t
    movable_from_solution(Game* g, const char* solutionStr, uint8_t step, PlayGround* movables) {
    const char solX = solutionStr[step * 2];
    const char solY = solutionStr[step * 2 + 1];

    int x, y;
    uint8_t dir;

    x = solX - 'a';
    if(solX <= 'Z') {
        dir = MOVABLE_LEFT;
        x = solX - 'A';
    }
    y = solY - 'a';
    if(solY <= 'Z') {
        dir = MOVABLE_RIGHT;
        y = solY - 'A';
    }

    if(x < 0 || x >= SIZE_X || y < 0 || y >= SIZE_Y) {
        end_solution(g);
        return 0;
    }

    clear_board(movables);
    (*movables)[y][x] = dir;

    return coord_from(x, y);
}

//-----------------------------------------------------------------------------

void start_solution(Game* g) {
    copy_level(g->boardBackup, g->board);

    clear_board(&g->board);
    load_game_board(g);

    g->currentMovableBackup = g->currentMovable;
    g->solutionStep = 0;
    g->solutionTotal = furi_string_size(g->levelData->solution) / 2;
    g->solutionMode = true;
    if(solution_will_have_penalty(g)) {
        g->levelSet->scores[g->currentLevel].spoiled = true;
        save_set_scores(g->levelSet->id, g->levelSet->scores);
        recalc_score(g);
    }
    solution_select(g);
}

//-----------------------------------------------------------------------------

void end_solution(Game* g) {
    g->state = SELECT_BRICK;
    g->currentMovable = g->currentMovableBackup;
    copy_level(g->board, g->boardBackup);
    clear_board(&g->toAnimate);
    map_movability(&g->board, &g->movables);
    update_board_stats(&g->board, g->stats);
    g->solutionMode = false;
}

//-----------------------------------------------------------------------------

void solution_select(Game* g) {
    g->currentMovable = movable_from_solution(
        g, furi_string_get_cstr(g->levelData->solution), g->solutionStep, &g->movables);
    g->move.frameNo = 35;
    g->state = SOLUTION_SELECT;
}

//-----------------------------------------------------------------------------

void solution_move(Game* g) {
    const uint8_t dir = movable_dir(&g->movables, g->currentMovable);
    start_move(g, dir);
}

//-----------------------------------------------------------------------------

void solution_next(Game* g) {
    if(g->solutionStep < g->solutionTotal - 1) {
        g->solutionStep++;
        solution_select(g);
    } else {
        end_solution(g);
    }
}

//-----------------------------------------------------------------------------

bool solution_will_have_penalty(Game* g) {
    return (g->levelSet->scores[g->currentLevel].moves == 0) &&
           (!g->levelSet->scores[g->currentLevel].spoiled);
}
