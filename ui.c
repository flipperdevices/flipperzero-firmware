#include "ui.h"

#include <gui/icon_i.h>
#include "fonts.h"
#include "game_vexed_icons.h"

void elements_button_right_back(Canvas* canvas, const char* str) {
    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 3;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const Icon* icon = &I_back_btn_10x8;
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = canvas_width(canvas);
    const uint8_t y = canvas_height(canvas);

    canvas_draw_box(canvas, x - button_width, y - button_height, button_width, button_height);
    canvas_draw_line(canvas, x - button_width - 1, y, x - button_width - 1, y - button_height + 0);
    canvas_draw_line(canvas, x - button_width - 2, y, x - button_width - 2, y - button_height + 1);
    canvas_draw_line(canvas, x - button_width - 3, y, x - button_width - 3, y - button_height + 2);

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, x - button_width + horizontal_offset, y - vertical_offset, str);
    canvas_draw_icon(
        canvas, x - horizontal_offset - icon->width, y - icon_v_offset, &I_back_btn_10x8);
    canvas_invert_color(canvas);
}

size_t
    elements_get_max_chars_to_fit(Canvas* canvas, Align horizontal, const char* text, uint8_t x) {
    const char* end = strchr(text, '\n');
    if(end == NULL) {
        end = text + strlen(text);
    }
    size_t text_size = end - text;
    FuriString* str;
    str = furi_string_alloc_set(text);
    furi_string_left(str, text_size);
    size_t result = 0;

    uint16_t len_px = canvas_string_width(canvas, furi_string_get_cstr(str));
    uint8_t px_left = 0;
    if(horizontal == AlignCenter) {
        if(x > (canvas_width(canvas) / 2)) {
            px_left = (canvas_width(canvas) - x) * 2;
        } else {
            px_left = x * 2;
        }
    } else if(horizontal == AlignLeft) {
        px_left = canvas_width(canvas) - x;
    } else if(horizontal == AlignRight) {
        px_left = x;
    } else {
        furi_crash();
    }

    if(len_px > px_left) {
        size_t excess_symbols_approximately =
            ceilf((float)(len_px - px_left) / ((float)len_px / (float)text_size));
        // reduce to 5 to be sure dash fit, and next line will be at least 5 symbols long
        if(excess_symbols_approximately > 0) {
            excess_symbols_approximately = MAX(excess_symbols_approximately, 5u);
            result = text_size - excess_symbols_approximately - 1;
        } else {
            result = text_size;
        }
    } else {
        result = text_size;
    }

    furi_string_free(str);
    return result;
}

void elements_multiline_text_aligned_limited(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t h,
    Align horizontal,
    Align vertical,
    const char* text) {
    furi_assert(canvas);
    furi_assert(text);

    uint8_t lines_count = 0;
    uint8_t lineNo = 0;
    uint8_t font_height = canvas_current_font_height(canvas);
    FuriString* line;

    /* go through text line by line and count lines */
    for(const char* start = text; start[0];) {
        size_t chars_fit = elements_get_max_chars_to_fit(canvas, horizontal, start, x);
        ++lines_count;
        start += chars_fit;
        start += start[0] == '\n' ? 1 : 0;
    }

    bool overflow = lines_count > h;
    lines_count = MIN(lines_count, h);

    if(vertical == AlignBottom) {
        y -= font_height * (lines_count - 1);
    } else if(vertical == AlignCenter) {
        y -= (font_height * (lines_count - 1)) / 2;
    }

    /* go through text line by line and print them */
    for(const char* start = text; start[0];) {
        size_t chars_fit = elements_get_max_chars_to_fit(canvas, horizontal, start, x);
        lineNo++;
        if((start[chars_fit] == '\n') || (start[chars_fit] == 0)) {
            line = furi_string_alloc_printf("%.*s", chars_fit, start);
        } else if(((y + font_height) > canvas_height(canvas)) || ((lineNo >= h) && overflow)) {
            line = furi_string_alloc_printf("%.*s...\n", chars_fit, start);
        } else {
            line = furi_string_alloc_printf("%.*s\n", chars_fit, start);
        }
        canvas_draw_str_aligned(canvas, x, y, horizontal, vertical, furi_string_get_cstr(line));
        furi_string_free(line);
        y += font_height;
        if(y > canvas_height(canvas)) {
            break;
        }

        if(lineNo >= h) {
            break;
        }
        start += chars_fit;
        start += start[0] == '\n' ? 1 : 0;
    }
}

void hint_pill_single(Canvas* canvas, const char* str) {
    canvas_draw_rframe(canvas, 82, 53, 46, 11, 3);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_micro_tr);
    canvas_draw_str_aligned(canvas, 104, 56, AlignCenter, AlignTop, str);
}

void hint_pill_double(Canvas* canvas, const char* str1, const char* str2, const Icon* icon) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, 82, 49, 46, 15, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 86, 51, icon);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_micro_tr);
    canvas_draw_str_aligned(canvas, 110, 51, AlignCenter, AlignTop, str1);
    canvas_draw_str_aligned(canvas, 110, 57, AlignCenter, AlignTop, str2);
}