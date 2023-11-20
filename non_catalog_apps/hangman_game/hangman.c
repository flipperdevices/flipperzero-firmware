#include "helpers/hangman.h"

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    if(!app->menu_show || hangman_menu_selection(app)) {
        while(hangman_main_loop(app)) {
            if(!hangman_wait_close_window(app)) break;
        }
    }

    return 0;
}
