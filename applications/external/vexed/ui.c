#include "ui.h"

#include <gui/icon_i.h>
#include "fonts.h"
#include "game_vexed_icons.h"

//-----------------------------------------------------------------------------

const Icon* tile_to_icon(uint8_t tile, bool gameOver) {
    switch(tile) {
    case 1:
        return &I_a;
    case 2:
        return &I_b;
    case 3:
        return &I_c;
    case 4:
        return &I_alt_d;
    case 5:
        return &I_e;
    case 6:
        return &I_f;
    case 7:
        return &I_g;
    case 8:
        return &I_h;
    case 9:
    default:
        return gameOver ? &I_w_black : &I_w;
    };
}

//-----------------------------------------------------------------------------

void gray_canvas(Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    for(int x = 0; x < GUI_DISPLAY_WIDTH; x += 2) {
        for(int y = 0; y < GUI_DISPLAY_HEIGHT; y++) {
            canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
        }
    }
}

//-----------------------------------------------------------------------------

void set_bounding_box(
    BoundingBox* box,
    uint8_t offset_x,
    uint8_t offset_y,
    uint8_t width,
    uint8_t height) {
    furi_assert(box);
    box->offset_x = offset_x;
    box->offset_y = offset_y;
    box->width = width;
    box->height = height;
}

//-----------------------------------------------------------------------------

void mask_canvas(Canvas* const canvas, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h) {
    canvas_set_color(canvas, ColorWhite);
    for(uint8_t x = sx; x < sx + w; x += 2) {
        for(uint8_t y = sy; y < sy + h; y++) {
            canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
        }
    }
}

//-----------------------------------------------------------------------------

void canvas_draw_hline_dotted(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t w) {
    for(uint8_t i = x; i < x + w; i += 2) {
        canvas_draw_dot(canvas, i, y);
    }
}

//-----------------------------------------------------------------------------

void canvas_draw_vline_dotted(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t h) {
    for(uint8_t i = y; i < y + h; i += 2) {
        canvas_draw_dot(canvas, x, i);
    }
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

size_t elements_get_max_chars_to_fit(
    Canvas* canvas,
    BoundingBox* box,
    Align horizontal,
    const char* text,
    uint8_t x) {
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
        if(x > (box->width / 2)) {
            px_left = (box->width - x) * 2;
        } else {
            px_left = x * 2;
        }
    } else if(horizontal == AlignLeft) {
        px_left = box->width - x;
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

//-----------------------------------------------------------------------------

void elements_multiline_text_aligned_limited(
    Canvas* canvas,
    BoundingBox* box,
    uint8_t x,
    uint8_t y,
    uint8_t h,
    Align horizontal,
    Align vertical,
    const char* text) {
    furi_assert(canvas);
    furi_assert(box);
    furi_assert(text);

    uint8_t lines_count = 0;
    uint8_t lineNo = 0;
    uint8_t font_height = canvas_current_font_height(canvas);
    FuriString* line;

    /* go through text line by line and count lines */
    for(const char* start = text; start[0];) {
        size_t chars_fit = elements_get_max_chars_to_fit(canvas, box, horizontal, start, x);
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
        size_t chars_fit = elements_get_max_chars_to_fit(canvas, box, horizontal, start, x);
        lineNo++;
        if((start[chars_fit] == '\n') || (start[chars_fit] == 0)) {
            line = furi_string_alloc_printf("%.*s", chars_fit, start);
        } else if(((y + font_height) > canvas_height(canvas)) || ((lineNo >= h) && overflow)) {
            line = furi_string_alloc_printf("%.*s...\n", chars_fit, start);
        } else {
            line = furi_string_alloc_printf("%.*s\n", chars_fit, start);
        }
        canvas_draw_str_aligned(
            canvas,
            x + box->offset_x,
            y + box->offset_y,
            horizontal,
            vertical,
            furi_string_get_cstr(line));
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

//-----------------------------------------------------------------------------

void hint_pill_single(Canvas* canvas, const char* str) {
    canvas_draw_rframe(canvas, 82, 53, 46, 11, 3);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_micro_tr);
    canvas_draw_str_aligned(canvas, 104, 56, AlignCenter, AlignTop, str);
}

//-----------------------------------------------------------------------------

void hint_pill_double(Canvas* canvas, const char* str1, const char* str2, const Icon* icon) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, 82, 49, 46, 15, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 86, 51, icon);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_micro_tr);
    canvas_draw_str_aligned(canvas, 110, 51, AlignCenter, AlignTop, str1);
    canvas_draw_str_aligned(canvas, 110, 57, AlignCenter, AlignTop, str2);
}

//-----------------------------------------------------------------------------

void menu_pill(
    Canvas* canvas,
    int no,
    int count,
    bool selected,
    bool masked,
    const char* label,
    const Icon* icon) {
    UNUSED(count);

    uint8_t height = 12;
    uint8_t y_offset = 4;
    uint8_t menu_pad_v = 4;
    uint8_t menu_pad_h = 8;
    uint8_t menu_total_w = 114;

    uint8_t menu_off_x = (GUI_DISPLAY_WIDTH - menu_total_w) / 2;
    uint8_t col = no % 2;
    uint8_t row = no / 2;

    uint8_t width = (menu_total_w - menu_pad_h) / 2;

    uint8_t x = (col * (width + menu_pad_h)) + menu_off_x;
    uint8_t origin_x = x + (width / 2);

    uint8_t y = y_offset + row * (height + menu_pad_v);

    canvas_set_color(canvas, selected ? ColorWhite : ColorBlack);
    canvas_draw_rbox(canvas, x + 1, y + 1, width, height, 2);
    canvas_set_color(canvas, selected ? ColorBlack : ColorWhite);
    canvas_draw_rbox(canvas, x, y, width, height, 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, width, height, 2);
    canvas_set_color(canvas, selected ? ColorWhite : ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, origin_x + 6, y + (height / 2), AlignCenter, AlignCenter, label);

    canvas_draw_icon(canvas, x + 3, y + 2, icon);

    if(masked) {
        mask_canvas(canvas, x, y, width + 1, height + 1);
    }
}

//-----------------------------------------------------------------------------

void main_menu_indi(Canvas* canvas, uint8_t x, uint8_t y, uint8_t xicon, const Icon* icon) {
    const uint8_t height = 12;
    const uint8_t width = 13;

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x + 1, y + 1, width, height, 2);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, x, y, width, height, 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, width, height, 2);

    canvas_draw_icon(canvas, x + 4 + xicon, y + 2, icon);
}

//-----------------------------------------------------------------------------

void main_menu_pill(
    Canvas* canvas,
    uint8_t y,
    uint8_t w,
    bool selected,
    bool leftIni,
    bool rightIndi,
    const char* label) {
    uint8_t height = 12;

    uint8_t x = (GUI_DISPLAY_WIDTH - w) / 2;
    uint8_t origin_x = x + (w / 2);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x + 1, y + 1, w, height, 2);
    canvas_set_color(canvas, selected ? ColorBlack : ColorWhite);
    canvas_draw_rbox(canvas, x, y, w, height, 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, w, height, 2);
    canvas_set_color(canvas, selected ? ColorWhite : ColorBlack);
    canvas_draw_str_aligned(canvas, origin_x, y + (height / 2), AlignCenter, AlignCenter, label);

    canvas_set_color(canvas, ColorBlack);
    if(leftIni) canvas_draw_icon(canvas, x - 8, y + 3, &I_ButtonLeft_4x7);
    if(rightIndi) canvas_draw_icon(canvas, x + w + 5, y + 3, &I_ButtonRight_4x7);
}

//-----------------------------------------------------------------------------

#define HIST_BRICK_W 13
#define HIST_BRICK_H 18

#define HIST_SPC_X 6
#define HIST_SPC_Y 6
#define HIST_PAD_X 8
#define HIST_PAD_Y 8

//-----------------------------------------------------------------------------

void panel_histogram_elem(Canvas* canvas, uint8_t x, uint8_t y, uint8_t brick, uint8_t count) {
    int bufSize = 8;
    char buf[bufSize];

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, x + 2, y, tile_to_icon(brick, false));
    canvas_set_font(canvas, FontPrimary);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u", count);
    canvas_draw_str_aligned(
        canvas, x + (HIST_BRICK_W / 2), y + HIST_BRICK_H, AlignCenter, AlignBottom, buf);
}

//-----------------------------------------------------------------------------

void panel_histogram(Canvas* canvas, const char* bricks, const uint8_t* values) {
    const uint8_t len = strlen(bricks);

    if(len == 0) {
        return;
    }

    const uint8_t rows = (len > 4) ? 2 : 1;
    const uint8_t breakpoint = (len > 4) ? (len - (len / 2)) : len;
    const uint8_t widths[2] = {breakpoint, len - breakpoint};
    const uint8_t columns = MAX(widths[0], widths[1]);

    const uint8_t w = (columns * HIST_BRICK_W) + ((columns - 1) * HIST_SPC_X) + (2 * HIST_PAD_X);
    const uint8_t h = (rows * HIST_BRICK_H) + ((rows - 1) * HIST_SPC_Y) + (2 * HIST_PAD_Y);

    const uint8_t x = (GUI_DISPLAY_WIDTH - w) / 2;
    const uint8_t y = (GUI_DISPLAY_HEIGHT - h) / 2;

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x + 2, y + 2, w, h, 4);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, x, y, w, h, 4);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, w, h, 4);

    uint8_t i, row = 0, col = 0;
    uint8_t posx = x + HIST_PAD_X;
    uint8_t posy = y + HIST_PAD_Y;

    for(i = 0; i < len; i++) {
        panel_histogram_elem(canvas, posx, posy, (uint8_t)(bricks[i]), values[i]);
        col++;
        posx += (HIST_BRICK_W + HIST_SPC_X);
        if((rows > 1) && (i == breakpoint - 1)) {
            row++;
            col = 0;
            posx = (x + HIST_PAD_X) + (widths[0] - widths[1]) * ((HIST_BRICK_W + HIST_SPC_X) / 2);
            posy += (HIST_BRICK_H + HIST_SPC_Y);
        }
    }
}

//-----------------------------------------------------------------------------

uint8_t dialog_frame(
    Canvas* canvas,
    uint8_t w,
    uint8_t h,
    bool bigHeader,
    bool safeFrame,
    const char* label) {
    const uint8_t x = (GUI_DISPLAY_WIDTH - w) / 2;
    const uint8_t y = (53 - h) / 2;

    canvas_set_color(canvas, ColorWhite);
    if(safeFrame) canvas_draw_rbox(canvas, x - 3, y - 2, w + 7, h + 5, 3);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x + 1, y + 1, w, h, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, x, y, w, h, 3);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, w, h, 3);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x, y, w, 11 + (bigHeader ? 1 : 0), 3);
    canvas_draw_box(canvas, x, y + 3, w, (11 - 3) + (bigHeader ? 1 : 0));

    canvas_set_color(canvas, ColorWhite);

    if(bigHeader) {
        canvas_set_font(canvas, FontPrimary);
    } else {
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    }

    canvas_draw_str_aligned(canvas, GUI_DISPLAY_WIDTH / 2, y + 2, AlignCenter, AlignTop, label);

    return y + 12;
}