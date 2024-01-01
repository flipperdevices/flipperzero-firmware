#include "events.h"

#include "move.h"

//-----------------------------------------------------------------------------

void events_for_selection(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            //current_level--;
            //refresh_level();
            find_movable_left(&game->movables, &game->current_movable);
            break;
        case InputKeyRight:
            //current_level++;
            //refresh_level();
            find_movable_right(&game->movables, &game->current_movable);
            break;
        case InputKeyUp:
            //image_position.y -= 2;
            //whiteB = !whiteB;
            find_movable_up(&game->movables, &game->current_movable);
            break;
        case InputKeyDown:
            //image_position.y += 2;
            //whiteB = !whiteB;
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

    if(event->type == InputTypeLong) {
        switch(event->key) {
        case InputKeyOk:
            game->current_level++;
            refresh_level(game);
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
                game->state = MAIN_MENU;
                break;
            case 3: // skip
                game->current_level++;
                refresh_level(game);
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
            game->state = MAIN_MENU;
            break;
        case InputKeyOk:
            game->current_level++;
            refresh_level(game);
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
                start_game_at_level(game, 0);
                break;
            case CUSTOM:
                start_game_at_level(game, game->current_level);
                break;
            case CONTINUE:
            default:
                start_game_at_level(game, game->current_level);
                break;
            }
            break;
        case InputKeyLeft:
            if(game->main_menu_pos == 0) {
                if(game->main_menu_mode == CUSTOM) {
                    game->main_menu_mode = game->hasContinue ? CONTINUE : NEW_GAME;
                } else if(game->main_menu_mode == CONTINUE) {
                    game->main_menu_mode = NEW_GAME;
                } else {
                    game->main_menu_mode = CUSTOM;
                }
            }
            break;
        case InputKeyRight:
            if(game->main_menu_pos == 0) {
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
            if(game->main_menu_mode == CUSTOM) {
                game->main_menu_pos =
                    (game->main_menu_pos - 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyDown:
            if(game->main_menu_mode == CUSTOM) {
                game->main_menu_pos =
                    (game->main_menu_pos + 1 + MAIN_MENU_COUNT) % MAIN_MENU_COUNT;
            }
            break;
        case InputKeyBack:
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