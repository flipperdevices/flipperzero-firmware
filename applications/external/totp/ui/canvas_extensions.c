#include "canvas_extensions.h"

void canvas_draw_str_ex(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    const char* text,
    size_t text_length,
    const FontInfo* const font) {
    const char* p_ch = text;
    char ch;
    size_t i = 0;
    uint8_t offset_x = x;
    uint8_t char_width = font->char_info[0].width;
    uint8_t offset_x_inc = char_width + font->space_width;
    while(i < text_length && (ch = *p_ch) != 0) {
        if(ch >= font->start_char && ch <= font->end_char) {
            uint8_t char_index = ch - font->start_char;
            canvas_draw_xbm(
                canvas,
                offset_x,
                y,
                char_width,
                font->height,
                &font->data[font->char_info[char_index].offset]);
        }

        offset_x += offset_x_inc;

        p_ch++;
        i++;
    }
}