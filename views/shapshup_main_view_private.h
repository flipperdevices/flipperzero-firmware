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
#define DEFAULT_SCALE_STEP 1.00
#define SUBGHZ_RAW_TOP_SCALE 13
#define SUBGHZ_RAW_BOTTOM_SCALE 42
#define CHART_LOWEST_POINT 40
#define CHART_HIGHTEST_POINT 20
#define SUBGHZ_RAW_START_SCALE 0
#define SCREEN_WIDTH 128

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
} ShapShupShapeItem;

ARRAY_DEF(ShapShupShapeItemArray, ShapShupShapeItem, M_POD_OPLIST)

#define M_OPL_ShapShupShapeItemArray_t() ARRAY_OPLIST(ShapShupShapeItemArray, M_POD_OPLIST)

typedef struct {
    ShapShupShapeItemArray_t data;
} ShapShupShapeStruct;

struct ShapShupMainView {
    View* view;
    ShapShupMainViewCallback callback;
    ShapShupRawFile* raw_file;
    uint64_t offset;
    float scale;
    ShapShupShapeStruct* shape_list;
    uint8_t count_shapes;
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
    ShapShupShapeStruct* shape_list;
    uint8_t count_shapes;
} ShapShupMainViewModel;

void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model);
void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model);
uint64_t calc_offset_per_page(uint64_t total, uint64_t min_len, float scale);
void format_number(uint64_t n, char* out);
bool shapshup_main_view_input(InputEvent* event, void* context);
void shapshup_main_view_enter(void* context);
void shapshup_main_view_exit(void* context);
void shapshup_main_view_array_reset(ShapShupMainView* instance);
void shapshup_main_view_create_shapes(ShapShupMainView* instance, uint64_t offset_per_page);