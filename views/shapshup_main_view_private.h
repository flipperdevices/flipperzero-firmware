#pragma once

#include "shapshup_main_view.h"
#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define STATUS_BAR_Y_SHIFT 11
#define STATUS_BAR_INFO_SHIFT 9
#define SUBGHZ_READ_RAW_RSSI_HISTORY_SIZE 100
#define SCALE_STEP 0.2
#define DEFAULT_SCALE_STEP 0.01
#define SUBGHZ_RAW_TOP_SCALE 13
#define SUBGHZ_RAW_BOTTOM_SCALE 42
#define CHART_LOWEST_POINT 40
#define CHART_HIGHTEST_POINT 20
#define SUBGHZ_RAW_START_SCALE 0
#define SUBGHZ_RAW_END_SCALE 130

struct ShapShupMainView {
    View* view;
    ShapShupMainViewCallback callback;
    ShapShupRawFile* raw_file;
    uint64_t offset;
    float scale;
    void* context;
};

typedef struct {
    bool rssi_history_end;
    uint64_t offset;
    uint64_t offset_per_page;
    uint64_t calc_total_len;
    bool is_ms;
    float scale;
    ShapShupRawFile* raw_file;
} ShapShupMainViewModel;

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model);
void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model);
uint64_t calc_offset_per_page(uint64_t total, float scale);
bool shapshup_main_view_input(InputEvent* event, void* context);
void shapshup_main_view_enter(void* context);
void shapshup_main_view_exit(void* context);