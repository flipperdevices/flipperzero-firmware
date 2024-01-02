#pragma once

#include <gui/gui.h>
#include <u8g2/u8g2.h>
#include <toolbox/compress.h>

//--- workaround to access private API

#define GUI_DISPLAY_WIDTH 128
#define GUI_DISPLAY_HEIGHT 64

#define GUI_DISPLAY_CENTER_X 64

typedef struct u8g2_struct u8g2_t;

struct Canvas {
    u8g2_t fb;
    CanvasOrientation orientation;
    uint8_t offset_x;
    uint8_t offset_y;
    uint8_t width;
    uint8_t height;
    CompressIcon* compress_icon;
};

//---

const Icon* tile_to_icon(uint8_t tile, bool gameOver);

void my_canvas_frame_set(
    Canvas* canvas,
    uint8_t offset_x,
    uint8_t offset_y,
    uint8_t width,
    uint8_t height);
void gray_canvas(Canvas* const canvas);
void mask_canvas(Canvas* const canvas, uint8_t sx, uint8_t sy, uint8_t w, uint8_t h);
void canvas_draw_hline_dotted(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t w);

void elements_button_right_back(Canvas* canvas, const char* str);
void elements_multiline_text_aligned_limited(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t h,
    Align horizontal,
    Align vertical,
    const char* text);

void hint_pill_single(Canvas* canvas, const char* str);
void hint_pill_double(Canvas* canvas, const char* str1, const char* str2, const Icon* icon);

void menu_pill(
    Canvas* canvas,
    int no,
    int count,
    bool selected,
    bool masked,
    const char* label,
    const Icon* icon);

void main_menu_pill(
    Canvas* canvas,
    uint8_t y,
    uint8_t w,
    bool selected,
    bool leftIni,
    bool rightIndi,
    const char* label);

void panel_histogram(Canvas* canvas, const char* bricks, const uint8_t* values);