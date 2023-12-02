#include "hangman.h"

size_t hangman_string_length(const char* str) {
    CONST furi_str = furi_string_alloc_set_str(str);
    CONST len = furi_string_utf8_length(furi_str);
    furi_string_free(furi_str);

    return len;
}

void hangman_draw_utf8_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str) {
    FuriStringUTF8State state = FuriStringUTF8StateStarting;
    FuriStringUnicodeValue value = 0;

    for(; *str; str++) {
        furi_string_utf8_decode(*str, &state, &value);
        if(state == FuriStringUTF8StateError) furi_crash(NULL);

        if(state == FuriStringUTF8StateStarting) {
            canvas_draw_glyph(canvas, x, y, value);
            // Only one-byte glyphs supported by canvas_glyph_width
            x += canvas_glyph_width(canvas, value <= 0xFF ? value : ' ');
        }
    }
}

void hangman_draw_gallows(Canvas* canvas, HangmanApp* app) {
    const Icon* gallows[HANGMAN_GALLOWS_MAX_STATE] = {&I_1, &I_2, &I_3, &I_4, &I_5, &I_6, &I_7};
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, 0, 30, gallows[app->gallows_state]);
    canvas_set_color(canvas, ColorWhite);
}

// This function was copied from Flipper Zero firmware
void hangman_ok_button(Canvas* canvas, uint8_t y, const char* str) {
    CONST button_height = 12;
    CONST vertical_offset = 3;
    CONST horizontal_offset = 1;
    CONST string_width = canvas_glyph_width(canvas, ' ') * hangman_string_length(str);
    CONST icon = &I_button_ok_7x7;
    CONST icon_h_offset = 3;
    CONST icon_width_with_offset = 7 + icon_h_offset;
    CONST icon_v_offset = 7 + vertical_offset;
    CONST button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    CONST x = (canvas_width(canvas) - button_width) / 2;

    canvas_draw_box(canvas, x, y - button_height, button_width, button_height);

    canvas_draw_line(canvas, x - 1, y, x - 1, y - button_height + 0);
    canvas_draw_line(canvas, x - 2, y, x - 2, y - button_height + 1);
    canvas_draw_line(canvas, x - 3, y, x - 3, y - button_height + 2);

    canvas_draw_line(canvas, x + button_width + 0, y, x + button_width + 0, y - button_height + 0);
    canvas_draw_line(canvas, x + button_width + 1, y, x + button_width + 1, y - button_height + 1);
    canvas_draw_line(canvas, x + button_width + 2, y, x + button_width + 2, y - button_height + 2);

    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, x + horizontal_offset, y - icon_v_offset, icon);
    hangman_draw_utf8_str(
        canvas, x + horizontal_offset + icon_width_with_offset, y - vertical_offset, str);
    canvas_invert_color(canvas);
}

void hangman_window(Canvas* canvas, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x - 2, y - 2, w + 4, h + 4);
    canvas_set_color(canvas, ColorBlack);

    elements_frame(canvas, x, y, w, h);
}

void hangman_text_window(Canvas* canvas, char* ok, char* txt) {
    CONST txt_w = canvas_glyph_width(canvas, ' ') * hangman_string_length(txt);

    CONST cw = canvas_width(canvas);
    CONST w = txt_w + 10;
    CONST h = 34;
    CONST y = 18;
    CONST x = (cw - w) / 2;

    hangman_window(canvas, x, y, w, h);
    hangman_ok_button(canvas, y + h, ok);

    hangman_draw_utf8_str(
        canvas, (cw - txt_w) / 2, y + canvas_current_font_height(canvas) + 4, txt);
}
