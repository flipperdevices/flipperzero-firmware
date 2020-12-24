#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <gui/canvas.h>
#include <flipper_v2.h>

typedef enum {
    // Idle screen ID's
    DolphinViewIdleMain,
    DolphinViewIdleStats,
    DolphinViewIdleDebug,
} DolphinViewIdle;

typedef struct {
    uint32_t icounter;
    uint32_t butthurt;
} DolphinViewIdleStatsModel;

void dolphin_view_idle_main_draw(Canvas* canvas, void* model);
void dolphin_view_idle_stats_draw(Canvas* canvas, void* model);
void dolphin_view_idle_debug_draw(Canvas* canvas, void* model);
uint32_t dolphin_view_idle_back(void* context);

typedef enum {
    DolphinViewFirstStartFirst,
    DolphinViewFirstStartSecond,
    DolphinViewFirstStartThird,
    DolphinViewFirstStartFourth,
} DolphinViewFirstStart;

void dolphin_view_first_start_first_draw(Canvas* canvas, void* model);
uint32_t dolphin_view_first_start_first_next(void* context);

uint32_t dolphin_view_first_start_second_previous(void* context);
void dolphin_view_first_start_second_draw(Canvas* canvas, void* model);
uint32_t dolphin_view_first_start_second_next(void* context);

uint32_t dolphin_view_first_start_third_previous(void* context);
void dolphin_view_first_start_third_draw(Canvas* canvas, void* model);
uint32_t dolphin_view_first_start_third_next(void* context);

uint32_t dolphin_view_first_start_fourth_previous(void* context);
void dolphin_view_first_start_fourth_draw(Canvas* canvas, void* model);
uint32_t dolphin_view_first_start_fourth_next(void* context);

typedef enum {
    DolphinViewProfileMain,
    DolphinViewProfileDetails,
} DolphinViewProfile;

void dolphin_view_profile_main_draw(Canvas* canvas, void* model);
void dolphin_view_profile_details_draw(Canvas* canvas, void* model);
