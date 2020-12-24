#include "dolphin_views.h"
#include <gui/view.h>

void dolphin_view_idle_main_draw(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon_name(canvas, 128 - 80, 0, I_Flipper_young_80x60);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 10, "/\\: Stats");
    canvas_draw_str(canvas, 5, 32, "OK: Menu");
    canvas_draw_str(canvas, 2, 52, "\\/: Version");
}

void dolphin_view_idle_stats_draw(Canvas* canvas, void* model) {
    DolphinViewIdleStatsModel* m = model;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Dolphin stats:");

    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, 64, "Icounter: %ld", m->icounter);
    canvas_draw_str(canvas, 5, 22, buffer);
    snprintf(buffer, 64, "Butthurt: %ld", m->butthurt);
    canvas_draw_str(canvas, 5, 32, buffer);
    canvas_draw_str(canvas, 5, 40, "< > change icounter");
}

void dolphin_view_idle_debug_draw(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Version info:");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 22, TARGET " " BUILD_DATE);
    canvas_draw_str(canvas, 5, 32, GIT_BRANCH);
    canvas_draw_str(canvas, 5, 42, GIT_BRANCH_NUM);
    canvas_draw_str(canvas, 5, 52, GIT_COMMIT);
}

uint32_t dolphin_view_idle_back(void* context) {
    return DolphinViewIdleMain;
}

// First 
void dolphin_view_first_start_first_draw(Canvas* canvas, void* model) {

}

uint32_t dolphin_view_first_start_first_next(void* context) {
    return DolphinViewFirstStartSecond;
}

// Second
void dolphin_view_first_start_second_draw(Canvas* canvas, void* model) {
}

uint32_t dolphin_view_first_start_second_previous(void* context) {
    return DolphinViewFirstStartFirst;
}

uint32_t dolphin_view_first_start_second_next(void* context) {
    return DolphinViewFirstStartThird;
}

// Third
void dolphin_view_first_start_third_draw(Canvas* canvas, void* model) {

}
uint32_t dolphin_view_first_start_third_previous(void* context) {
    return DolphinViewFirstStartSecond;
}
uint32_t dolphin_view_first_start_third_next(void* context){
    return DolphinViewFirstStartFourth;
}

// Fourth
void dolphin_view_first_start_fourth_draw(Canvas* canvas, void* model) {

}
uint32_t dolphin_view_first_start_fourth_previous(void* context) {
    return DolphinViewFirstStartThird;
}
uint32_t dolphin_view_first_start_fourth_next(void* context) {
    return VIEW_DESTROY;
}