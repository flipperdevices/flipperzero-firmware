#include "events.h"

#include "move.h"

//-----------------------------------------------------------------------------

void events_for_selection(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            find_movable_left(&game->movables, &game->current_movable);
            break;
        case InputKeyRight:
            find_movable_right(&game->movables, &game->current_movable);
            break;
        case InputKeyUp:
            find_movable_up(&game->movables, &game->current_movable);
            break;
        case InputKeyDown:
            find_movable_down(&game->movables, &game->current_movable);
            break;
        case InputKeyOk:
            click_selected(game);
            break;
        case InputKeyBack:
            game->menu_paused_pos = (game->undo_movable == MOVABLE_NOT_FOUND) ? 4 : 0;
            game->state = PAUSED;
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_direction(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            start_move(game, MOVABLE_LEFT);
            break;
        case InputKeyRight:
            start_move(game, MOVABLE_RIGHT);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_paused(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            game->menu_paused_pos =
                (game->menu_paused_pos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos =
                    (game->menu_paused_pos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyRight:
            game->menu_paused_pos = (game->menu_paused_pos + 1) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos = (game->menu_paused_pos + 1) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyUp:
            game->menu_paused_pos =
                (game->menu_paused_pos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos =
                    (game->menu_paused_pos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyDown:
            game->menu_paused_pos = (game->menu_paused_pos + 2) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos = (game->menu_paused_pos + 2) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyOk:
            switch(game->menu_paused_pos) {
            case 0: // undo
                undo(game);
                break;
            case 1: // restart
                refresh_level(game);
                break;
            case 2: // menu
                game->main_menu_mode = CUSTOM;
                game->main_menu_btn = MODE_BTN;
                game->state = MAIN_MENU;
                break;
            case 3: // skip
                start_game_at_level(game, game->currentLevel + 1);
                break;
            case 4: // count
                game->state = HISTOGRAM;
                break;
            case 5: // solve
            default:
                break;
            }
            break;
        case InputKeyBack:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_game_over(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyBack:
            undo(game);
            break;
        case InputKeyOk:
            game->main_menu_mode = NEW_GAME;
            game->state = MAIN_MENU;
            break;
        case InputKeyLeft:
            refresh_level(game);
            break;
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_level_finished(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
            game->main_menu_mode = CONTINUE;
            game->state = MAIN_MENU;
            break;
        case InputKeyOk:
            start_game_at_level(game, game->currentLevel + 1);
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_histogram(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_main_menu(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyOk:
            switch(game->main_menu_mode) {
            case NEW_GAME:
                forget_continue(game);
                FuriString* setName = furi_string_alloc_set(assetLevels[0]);
                load_gameset_if_needed(game, setName);
                furi_string_free(setName);
                start_game_at_level(game, 0);
                break;
            case CUSTOM:
                switch(game->main_menu_btn) {
                case LEVELSET_BTN:
                case LEVELNO_BTN:
                    if(game->mainMenuInfo) {
                        game->mainMenuInfo = false;
                        load_gameset_if_needed(game, game->selectedSet);
                        start_game_at_level(game, game->selectedLevel);
                    } else {
                        game->mainMenuInfo = true;
                    }
                    break;
                default:
                case MODE_BTN:
                    load_gameset_if_needed(game, game->selectedSet);
                    start_game_at_level(game, game->selectedLevel);
                    break;
                }
                break;
            case CONTINUE:
            default:
                load_gameset_if_needed(game, game->continueSet);
                start_game_at_level(game, game->continueLevel + 1);
                break;
            }
            break;
        case InputKeyLeft:
            if(game->mainMenuInfo) return;
            switch(game->main_menu_btn) {
            case LEVELSET_BTN:
                game->setPos = (game->setPos > 0) ? game->setPos - 1 : game->setCount - 1;
                furi_string_set(game->selectedSet, assetLevels[game->setPos]);
                load_gameset_if_needed(game, game->selectedSet);
                game->selectedLevel = 0;
                break;
            case LEVELNO_BTN:
                game->selectedLevel = (game->selectedLevel > 0) ? game->selectedLevel - 1 :
                                                                  game->levelSet->maxLevel - 1;
                break;
            case MODE_BTN:
            default:
                if(game->main_menu_mode == CUSTOM) {
                    game->main_menu_mode = game->hasContinue ? CONTINUE : NEW_GAME;
                } else if(game->main_menu_mode == CONTINUE) {
                    game->main_menu_mode = NEW_GAME;
                } else {
                    game->main_menu_mode = CUSTOM;
                }
                break;
            }
            break;
        case InputKeyRight:
            if(game->mainMenuInfo) return;
            switch(game->main_menu_btn) {
            case LEVELSET_BTN:
                game->setPos = (game->setPos < game->setCount - 1) ? game->setPos + 1 : 0;
                furi_string_set(game->selectedSet, assetLevels[game->setPos]);
                load_gameset_if_needed(game, game->selectedSet);
                game->selectedLevel = 0;
                break;
            case LEVELNO_BTN:
                game->selectedLevel = (game->selectedLevel < (game->levelSet->maxLevel - 1)) ?
                                          game->selectedLevel + 1 :
                                          0;
                break;
            case MODE_BTN:
            default:

                if(game->main_menu_mode == NEW_GAME) {
                    game->main_menu_mode = game->hasContinue ? CONTINUE : CUSTOM;
                } else if(game->main_menu_mode == CONTINUE) {
                    game->main_menu_mode = CUSTOM;
                } else {
                    game->main_menu_mode = NEW_GAME;
                }
            }
            break;
        case InputKeyUp:
            if(game->mainMenuInfo) return;
            if(game->main_menu_mode == CUSTOM) {
                game->main_menu_btn =
                    (game->main_menu_btn - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyDown:
            if(game->mainMenuInfo) return;
            if(game->main_menu_mode == CUSTOM) {
                game->main_menu_btn =
                    (game->main_menu_btn + 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyBack:
            if(game->mainMenuInfo) {
                game->mainMenuInfo = false;
            }
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_intro(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyOk:
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
            game->state = MAIN_MENU;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_game(InputEvent* event, Game* game) {
    switch(game->state) {
    case MAIN_MENU:
        events_for_main_menu(event, game);
        break;
    case INTRO:
        events_for_intro(event, game);
        break;
    case SELECT_BRICK:
        events_for_selection(event, game);
        break;
    case SELECT_DIRECTION:
        events_for_direction(event, game);
        break;
    case PAUSED:
        events_for_paused(event, game);
        break;
    case HISTOGRAM:
        events_for_histogram(event, game);
        break;
    case GAME_OVER:
        events_for_game_over(event, game);
        break;
    case LEVEL_FINISHED:
        events_for_level_finished(event, game);
        break;
    default:
        break;
    }
}