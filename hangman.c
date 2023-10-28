#include "helpers/hangman.h"
#include <u8g2/u8g2_fonts.c>

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* context = (HangmanApp*) ctx;

    canvas_clear(canvas);
    hangman_draw_word(canvas, context);
    canvas_draw_icon(canvas, 0, 30, &I_hangman);

    hangman_draw_keyboard(canvas, context);
}

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    hangman_wait_a_key(app);

    return 0;
}
