#pragma once

#include "shapshup_main_view.h"
#include "../shapshup_i.h"
#include <input/input.h>
#include <gui/elements.h>
#include "assets_icons.h"
#include <gui/icon.h>
#include <m-array.h>

#define STATUS_BAR_Y_SHIFT 11
#define STATUS_BAR_INFO_SHIFT 9
#define SCALE_STEP 0.2f
#define SHAPSHUP_DEFAULT_SCALE_STEP 1.00f
#define SHAPSHUP_TOP_SCALE 13
#define SHAPSHUP_BOTTOM_SCALE 42
#define SHAPSHUP_CHART_LOWEST_POINT 40
#define SHAPSHUP_CHART_HIGHEST_POINT 20
#define SCREEN_WIDTH 128

/**
 * @brief ShapShupShapeItem
 * uint8_t x;
 * uint8_t y;
 * uint8_t width;
 * uint8_t height;
 */
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
} ShapShupShapeItem;

ARRAY_DEF(ShapShupShapeItemArray, ShapShupShapeItem, M_POD_OPLIST)

#define M_OPL_ShapShupShapeItemArray_t() ARRAY_OPLIST(ShapShupShapeItemArray, M_POD_OPLIST)

/**
 * @brief ShapShupShapeStruct
 * ShapShupShapeItemArray_t data
 * 
 */
typedef struct {
    ShapShupShapeItemArray_t data;
} ShapShupShapeStruct;

/**
 * @brief ShapShupMainView
 * View* view;
 * ShapShupMainViewCallback callback;
 * ShapShupRawFile* raw_file;
 * uint64_t offset;
 * float scale;
 * ShapShupShapeStruct* shape_list;
 * uint8_t count_shapes;
 * void* context;
 * 
 */
struct ShapShupMainView {
    View* view;
    ShapShupMainViewCallback callback;
    ShapShupRawFile* raw_file;
    uint64_t offset;
    float scale;
    ShapShupShapeStruct* shape_list;
    uint8_t count_shapes;
    uint64_t alert_dismiss_time;
    void* context;
};

/**
 * @brief ShapShupMainViewModel
 * bool rssi_history_end;
 * uint64_t offset;
 * uint64_t offset_per_page;
 * uint64_t calc_total_len;
 * bool is_ms;
 * float scale;
 * ShapShupRawFile* raw_file;
 * ShapShupShapeStruct* shape_list;
 * uint8_t count_shapes;
 * 
 */
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
    uint64_t alert_dismiss_time;
    char alert_text[SHAPSHUP_TEXT_STORE_SIZE];
} ShapShupMainViewModel;

void shapshup_main_view_show_alert(ShapShupMainView* instance, const char* text, uint32_t ttl);
void shapshup_main_view_reset_alert(ShapShupMainView* instance);
void shapshup_main_view_draw_alert_if_needed(Canvas* canvas, ShapShupMainViewModel* model);

/**
 * @brief shapshup_main_view_draw
 * 
 * @param canvas Canvas*
 * @param model  ShapShupMainViewModel*
 */
void shapshup_main_view_draw(Canvas* canvas, ShapShupMainViewModel* model);

/**
 * @brief shapshup_main_view_draw_scale
 * 
 * @param canvas Canvas*
 * @param model  ShapShupMainViewModel*
 */
void shapshup_main_view_draw_scale(Canvas* canvas, ShapShupMainViewModel* model);

/**
 * @brief calc_offset_per_page
 * 
 * @param total 
 * @param min_len 
 * @param scale 
 * @return uint64_t 
 */
uint64_t calc_offset_per_page(uint64_t total, uint64_t min_len, float scale);

/**
 * @brief format_number
 * 
 * @param n 
 * @param out 
 */
void format_number(uint64_t n, char* out);

/**
 * @brief shapshup_main_view_input
 * 
 * @param event 
 * @param context 
 * @return true 
 * @return false 
 */
bool shapshup_main_view_input(InputEvent* event, void* context);

/**
 * @brief shapshup_main_view_enter
 * 
 * @param context 
 */
void shapshup_main_view_enter(void* context);

/**
 * @brief shapshup_main_view_exit
 * 
 * @param context 
 */
void shapshup_main_view_exit(void* context);

/**
 * @brief shapshup_main_view_array_reset
 * 
 * @param instance ShapShupMainView*
 */
void shapshup_main_view_array_reset(ShapShupMainView* instance);

/**
 * @brief shapshup_main_view_create_shapes
 * 
 * @param instance ShapShupMainView*
 * @param offset_per_page 
 */
void shapshup_main_view_create_shapes(ShapShupMainView* instance, uint64_t offset_per_page);
