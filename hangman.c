#include "helpers/hangman.h"
#include <u8g2/u8g2_fonts.c>

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* app = (HangmanApp*) ctx;

    canvas_clear(canvas);

    hangman_draw_word(canvas, app);
    hangman_draw_gallows(canvas, app);
    hangman_draw_keyboard(canvas, app);

    if (app->eog != HangmanGameOn) {
        if(app->eog == HangmanGameLoose) {
            hangman_text_window(canvas, "Вы проиграли!");
        } else {
            hangman_text_window(canvas, "Вы выиграли!");
        }
        app->need_generate = true;
    }
}

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    while (hangman_main_loop(app)) {
        if (!hangman_wait_close_window(app)) break;
    }

    return 0;
}
