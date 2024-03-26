#include "doc_view_common.h"

void doc_draw_text(Canvas* canvas, FuriString* text,
    uint8_t x_one, uint8_t x_two, uint8_t y,
    uint8_t font_height, uint8_t start_index, uint8_t max_index) {
    furi_assert(canvas);

    FuriString* str = furi_string_alloc();
    const char* start = furi_string_get_cstr(text);
    char* mid;
    char* end;
    uint8_t temp_index = 0;

    do {
        mid = strchr(start, '\t');
        end = strchr(start, '\n');

        if(mid && end && 0 < end - mid) {
            furi_string_set_strn(str, start, mid - start);

            if(temp_index >= start_index) {
                canvas_draw_str_aligned(canvas, x_one, y, AlignLeft, AlignBottom, furi_string_get_cstr(str));
            }
        }

        if(end && mid) {
            furi_string_set_strn(str, mid, end - start);
            start = end + 1;
        } else if(end) {
            furi_string_set_strn(str, start, end - start);
            start = end + 1;
        } else {
            furi_string_set(str, start);
        }

        if(temp_index >= start_index) {
            canvas_draw_str_aligned(canvas, x_two, y, AlignRight, AlignBottom, furi_string_get_cstr(str));
            y += font_height;
        }

        temp_index++;
    } while(end && y < 64 && temp_index <= start_index + max_index - 1);

    furi_string_free(str);
}