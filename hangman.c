#include "helpers/hangman.h"
#include <u8g2/u8g2_fonts.c>

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* context = (HangmanApp*) ctx;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_haxrcorp4089_t_cyrillic);

    uint8_t h = canvas_current_font_height(canvas);
    hangman_draw_utf8_str(canvas, 0, h, context->word);
}

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    hangman_wait_a_key(app);

    return 0;
}
