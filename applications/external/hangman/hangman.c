#include "helpers/hangman.h"
#include <u8g2/u8g2_fonts.c>

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    while(hangman_main_loop(app)) {
        if(!hangman_wait_close_window(app)) break;
    }

    return 0;
}
