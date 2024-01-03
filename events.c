#include "events.h"

#include "move.h"
#include "game.h"

//-----------------------------------------------------------------------------

void events_for_selection(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        if(game->solutionMode) {
            end_solution(game);
            return;
        }
        switch(event->key) {
        case InputKeyLeft:
            find_movable_left(&game->movables, &game->currentMovable);
            break;
        case InputKeyRight:
            find_movable_right(&game->movables, &game->currentMovable);
            break;
        case InputKeyUp:
            find_movable_up(&game->movables, &game->currentMovable);
            break;
        case InputKeyDown:
            find_movable_down(&game->movables, &game->currentMovable);
            break;
        case InputKeyOk:
            click_selected(game);
            break;
        case InputKeyBack:
            game->menuPausedPos = (game->undoMovable == MOVABLE_NOT_FOUND) ? 4 : 0;
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
            game->menuPausedPos =
                (game->menuPausedPos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            if((game->menuPausedPos == 0) && (game->undoMovable == MOVABLE_NOT_FOUND)) {
                game->menuPausedPos =
                    (game->menuPausedPos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyRight:
            game->menuPausedPos = (game->menuPausedPos + 1) % MENU_PAUSED_COUNT;
            if((game->menuPausedPos == 0) && (game->undoMovable == MOVABLE_NOT_FOUND)) {
                game->menuPausedPos = (game->menuPausedPos + 1) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyUp:
            game->menuPausedPos =
                (game->menuPausedPos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            if((game->menuPausedPos == 0) && (game->undoMovable == MOVABLE_NOT_FOUND)) {
                game->menuPausedPos =
                    (game->menuPausedPos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyDown:
            game->menuPausedPos = (game->menuPausedPos + 2) % MENU_PAUSED_COUNT;
            if((game->menuPausedPos == 0) && (game->undoMovable == MOVABLE_NOT_FOUND)) {
                game->menuPausedPos = (game->menuPausedPos + 2) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyOk:
            switch(game->menuPausedPos) {
            case 0: // undo
                undo(game);
                break;
            case 1: // restart
                refresh_level(game);
                break;
            case 2: // menu
                game->mainMenuMode = CUSTOM;
                game->mainMenuBtn = MODE_BTN;
                game->state = MAIN_MENU;
                break;
            case 3: // skip
                start_game_at_level(game, game->currentLevel + 1);
                break;
            case 4: // count
                game->state = HISTOGRAM;
                break;
            case 5: // solve
                if(solution_will_have_penalty(game)) {
                    game->state = SOLUTION_PROMPT;
                } else {
                    start_solution(game);
                }
                break;
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
            game->mainMenuMode = (game->hasContinue) ? CONTINUE : NEW_GAME;
            game->mainMenuBtn = MODE_BTN;
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
            game->mainMenuMode = CONTINUE;
            game->mainMenuBtn = MODE_BTN;
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
            switch(game->mainMenuMode) {
            case NEW_GAME:
                if(game->hasContinue) {
                    game->state = RESET_PROMPT;
                } else {
                    new_game(game);
                }
                break;
            case CUSTOM:
                switch(game->mainMenuBtn) {
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
            switch(game->mainMenuBtn) {
            case LEVELSET_BTN:
                game->setPos = (game->setPos > 0) ? game->setPos - 1 : game->setCount - 1;
                furi_string_set(game->selectedSet, level_on_pos(game, game->setPos));
                load_gameset_if_needed(game, game->selectedSet);
                game->selectedLevel = 0;
                break;
            case LEVELNO_BTN:
                game->selectedLevel = (game->selectedLevel > 0) ? game->selectedLevel - 1 :
                                                                  game->levelSet->maxLevel - 1;
                break;
            case MODE_BTN:
            default:
                if(game->mainMenuMode == CUSTOM) {
                    game->mainMenuMode = game->hasContinue ? CONTINUE : NEW_GAME;
                } else if(game->mainMenuMode == CONTINUE) {
                    game->mainMenuMode = NEW_GAME;
                } else {
                    game->mainMenuMode = CUSTOM;
                }
                break;
            }
            break;
        case InputKeyRight:
            if(game->mainMenuInfo) return;
            switch(game->mainMenuBtn) {
            case LEVELSET_BTN:
                game->setPos = (game->setPos < game->setCount - 1) ? game->setPos + 1 : 0;
                furi_string_set(game->selectedSet, level_on_pos(game, game->setPos));
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

                if(game->mainMenuMode == NEW_GAME) {
                    game->mainMenuMode = game->hasContinue ? CONTINUE : CUSTOM;
                } else if(game->mainMenuMode == CONTINUE) {
                    game->mainMenuMode = CUSTOM;
                } else {
                    game->mainMenuMode = NEW_GAME;
                }
            }
            break;
        case InputKeyUp:
            if(game->mainMenuInfo) return;
            if(game->mainMenuMode == CUSTOM) {
                game->mainMenuBtn = (game->mainMenuBtn - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyDown:
            if(game->mainMenuInfo) return;
            if(game->mainMenuMode == CUSTOM) {
                game->mainMenuBtn = (game->mainMenuBtn + 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyBack:
            if(game->mainMenuInfo) {
                game->mainMenuInfo = false;
            } else {
                game->state = ABOUT;
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

void events_for_about(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyOk:
            // handled on root level - exit
            // see: game_vexed.c
            break;
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

void events_for_solution_prompt(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyOk:
            start_solution(game);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
            game->state = SELECT_BRICK;
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_solution_select(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyUp:
        case InputKeyDown:
            break;
        case InputKeyOk:
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyBack:
            end_solution(game);
        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------

void events_for_reset(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyOk:
            new_game(game);
            break;
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

void events_for_invalid(InputEvent* event, Game* game) {
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
    case ABOUT:
        events_for_about(event, game);
        break;
    case RESET_PROMPT:
        events_for_reset(event, game);
        break;
    case INVALID_PROMPT:
        events_for_invalid(event, game);
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
    case SOLUTION_PROMPT:
        events_for_solution_prompt(event, game);
        break;
    case SOLUTION_SELECT:
        events_for_solution_select(event, game);
        break;
    case GAME_OVER:
        events_for_game_over(event, game);
        break;
    case LEVEL_FINISHED:
        events_for_level_finished(event, game);
        break;
    case MOVE_SIDES:
    case MOVE_GRAVITY:
    case EXPLODE:
        if(game->solutionMode) {
            events_for_solution_select(event, game);
        }
    default:
        break;
    }
}