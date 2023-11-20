#include "hangman.h"

bool hangman_wait_close_window(HangmanApp* app) {
    InputEvent event;

    for(;;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_clear_state(app);
                    view_port_update(app->view_port);
                    return true;

                case InputKeyBack:
                    return false;
                default:
                    break;
                }
            }
        }
    }
}

bool hangman_menu_selection(HangmanApp* app) {
    InputEvent event;
    int8_t menu_cnt = app->menu_cnt / 2;

    for(;;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    app->menu_show = false;
                    hangman_load_lang(app);
                    hangman_clear_state(app);
                    view_port_update(app->view_port);
                    return true;

                case InputKeyBack:
                    return false;

                case InputKeyUp:
                    if(--app->menu_item < 0) {
                        app->menu_item = menu_cnt - 1;
                    }
                    break;
                case InputKeyDown:
                    if(++app->menu_item > menu_cnt - 1) {
                        app->menu_item = 0;
                    }
                    break;
                default:
                    break;
                }

                view_port_update(app->view_port);
            }
        }
    }
}

bool hangman_main_loop(HangmanApp* app) {
    InputEvent event;

    while(app->eog == HangmanGameOn && !app->need_generate) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_choice_letter(app);
                    break;

                case InputKeyBack:
                    return false;

                case InputKeyDown:
                    app->pos += app->lang->keyboard_cols;

                    if(app->pos >= app->lang->letters_cnt) {
                        app->pos %= app->lang->keyboard_cols;
                    }

                    break;

                case InputKeyUp:
                    if(app->pos >= app->lang->keyboard_cols) {
                        app->pos -= app->lang->keyboard_cols;
                    } else {
                        app->pos += app->lang->keyboard_cols * (app->lang->keyboard_rows - 1);
                        if(app->pos >= app->lang->letters_cnt) {
                            app->pos -= app->lang->keyboard_cols;
                        }
                    }
                    break;

                case InputKeyLeft:
                    if(app->pos > 0) {
                        app->pos--;
                    } else {
                        app->pos = app->lang->letters_cnt - 1;
                    }
                    break;

                case InputKeyRight:
                    if(app->pos < app->lang->letters_cnt - 1) {
                        app->pos++;
                    } else {
                        app->pos = 0;
                    }
                    break;

                default:
                    break;
                }

                view_port_update(app->view_port);
            }
        }
    }

    return true;
}