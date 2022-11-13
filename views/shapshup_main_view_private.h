#pragma once

#include "shapshup_main_view.h"
#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define STATUS_BAR_Y_SHIFT 14
#define SUBGHZ_READ_RAW_RSSI_HISTORY_SIZE 100
#define SCALE_STEP 0.2
#define SUBGHZ_RAW_TOP_SCALE 15
#define SUBGHZ_RAW_START_SCALE 5
#define SUBGHZ_RAW_END_SCALE 120

struct ShapShupMainView {
    View* view;
    ShapShupMainViewCallback callback;
    ShapShupRawFile* raw_file;
    uint32_t offset;
    float scale;
    void* context;
};

typedef struct {
    bool rssi_history_end;
    uint32_t offset;
    uint32_t offset_per_page;
    float scale;
    ShapShupRawFile* raw_file;
} ShapShupMainViewModel;

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model);
void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model);
uint32_t calc_offset_per_page(uint32_t total, float scale);
bool shapshup_main_view_input(InputEvent* event, void* context);
void shapshup_main_view_enter(void* context);
void shapshup_main_view_exit(void* context);
/**
 * Thanks to the author of metronome
 * @param canvas
 * @param str
 */
void elements_button_top_left(Canvas* canvas, const char* str);
/**
 * Thanks to the author of metronome
 * @param canvas
 * @param str
 */
void elements_button_top_right(Canvas* canvas, const char* str);