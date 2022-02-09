#include "assets_icons.h"
#include "furi_hal_delay.h"
#include <gui/icon.h>
#include <desktop/desktop_i.h>
#include <stdint.h>
#include <portmacro.h>
#include <FreeRTOS.h>
#include <stdbool.h>
#include <furi.h>
#include <FreeRTOS-Kernel/include/timers.h>
#include "locked_doors.h"

#define DOOR_OFFSET_START   -55
#define DOOR_OFFSET_END     0

#define DOOR_L_FINAL_POS 0
#define DOOR_R_FINAL_POS 60

// #define DOOR_L_POS -57       // dbg_
// #define DOOR_L_POS_MAX 0
// #define DOOR_R_POS 115
// #define DOOR_R_POS_MIN 60

struct DesktopViewsHelpersLockedDoors {
    bool animating;
    TimerHandle_t timer;
    DesktopViewsHelpersLockedDoorsCallback callback;
    void* callback_context;
    View* view;
    int8_t door_offset;
    bool finished;
};

void desktop_views_helpers_locked_doors_draw(Canvas* canvas, DesktopViewsHelpersLockedDoors* instance) {

    int8_t offset = instance->door_offset;
    uint8_t door_left_x = DOOR_L_FINAL_POS + offset;
    uint8_t door_right_x = DOOR_R_FINAL_POS - offset;

    uint8_t height = icon_get_width(&I_DoorLeft_70x55);
    canvas_draw_icon(canvas, door_left_x, canvas_height(canvas) - height, &I_DoorLeft_70x55);
    canvas_draw_icon(canvas, door_right_x, canvas_height(canvas) - height, &I_DoorRight_70x55);
}

bool desktop_views_helpers_locked_doors_move(DesktopViewsHelpersLockedDoors* instance) {
    bool stop = false;
    if (instance->door_offset >= DOOR_OFFSET_END) {
        stop = true;
    } else {
        instance->door_offset = CLAMP(instance->door_offset + 5, DOOR_OFFSET_END, DOOR_OFFSET_START);
    }

    return stop;
}

void desktop_views_helpers_locked_doors_reset(DesktopViewsHelpersLockedDoors* instance) {
    furi_assert(instance);

    instance->door_offset = DOOR_OFFSET_START;
    instance->finished = false;
}



static void desktop_views_helpers_locked_doors_timer_callback(TimerHandle_t timer) {
    DesktopViewsHelpersLockedDoors* instance = pvTimerGetTimerID(timer);

    /* no need to lock, cause value changed in 1 thread, and can be acquired
     * in another thread in consistent state */
    if (instance->door_offset >= DOOR_OFFSET_END) {
        xTimerStop(timer, 0);
        /* use 'finished' flag in case timer callback is called
         * again before timer is disabled */
        if (!instance->finished) {
            instance->finished = true;
            instance->callback(instance->callback_context);
        }
    } else {
        instance->door_offset = CLAMP(instance->door_offset + 5, DOOR_OFFSET_END, DOOR_OFFSET_START);
    }

    view_get_model(instance->view);
    view_commit_model(instance->view, true);
}

DesktopViewsHelpersLockedDoors* desktop_views_helpers_locked_doors_alloc(void) {
    DesktopViewsHelpersLockedDoors* instance = furi_alloc(sizeof(DesktopViewsHelpersLockedDoors));
    instance->animating = false;
    instance->timer = NULL;
    instance->callback = NULL;
    instance->callback_context = NULL;
    instance->timer = xTimerCreate("", 1000 / 16, pdTRUE, instance, desktop_views_helpers_locked_doors_timer_callback);

    return instance;
}

void desktop_views_helpers_locked_doors_free(DesktopViewsHelpersLockedDoors* instance) {
    furi_assert(instance);

    xTimerStop(instance->timer, portMAX_DELAY);
    while(xTimerIsTimerActive(instance->timer)) {
        delay(1);
    }
    xTimerDelete(instance->timer, portMAX_DELAY);
}

void desktop_views_helpers_locked_doors_restart(DesktopViewsHelpersLockedDoors* instance, View* view) {
    furi_assert(instance);
    furi_assert(view);

    instance->door_offset = DOOR_OFFSET_START;
    instance->finished = false;
    xTimerStart(instance->timer, portMAX_DELAY);
}

void desktop_views_helpers_locked_doors_set_callback(DesktopViewsHelpersLockedDoors* instance, DesktopViewsHelpersLockedDoorsCallback callback, void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->callback_context = context;
}

