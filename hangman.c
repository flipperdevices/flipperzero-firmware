#include "helpers/hangman.h"
#include <u8g2/u8g2_fonts.c>

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* context = (HangmanApp*) ctx;

    canvas_clear(canvas);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);

    uint8_t glyph_w  = hangman_GetGlyphWidth(&canvas->fb, 0x20);
    uint8_t center_x = (canvas_width(canvas) - glyph_w * strlen(context->word) / 2) / 2;

    uint8_t h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);
    hangman_draw_utf8_str(canvas, center_x, h, context->word);

    canvas_set_color(canvas, ColorXOR);
    hangman_draw_utf8_str(canvas, center_x, h, context->word_guessed);

    canvas_draw_icon(canvas, 0, 30, &I_hangman);

    hangman_draw_keyboard(canvas);
}

int32_t hangman_main(void* p) {
    UNUSED(p);
    __attribute__((__cleanup__(hangman_app_free))) HangmanApp* app = hangman_app_alloc();

    hangman_wait_a_key(app);

    return 0;
}
