#pragma once

/**
 * @file button_monitor.h
 * @brief This file contains the ButtonMonitor type and related functions.
 * @details This file contains the ButtonMonitor type and related functions.
 * The ButtonMonitor type is used to monitor the state of the buttons on the
 * Flipboard. The ButtonMonitor runs in its own thread and calls a callback
 * when a button event occurs.
*/

#include <furi.h>
#include <furi_hal.h>

typedef struct ButtonMonitor ButtonMonitor;
typedef bool (*ButtonMonitorEventCallback)(void* context, uint8_t old_key, uint8_t new_key);

typedef enum {
    SwitchId1 = 1,
    SwitchId2 = 1 << 1,
    SwitchId3 = 1 << 2,
    SwitchId4 = 1 << 3,
} SwitchIds;

#define IS_SWITCH_PRESSED(key, switch_id) ((key & switch_id) == switch_id)
#define IS_SWITCH_RELEASED(key, switch_id) ((key & switch_id) == 0)
#define IS_SWITCH_ALL_RELEASED(key) (key == 0)

/**
 * @brief Allocates a new button monitor.
 * @details Allocates a new button monitor. The button monitor 
 * will start running immediately, but the callback will be NULL.
 * @return The new button monitor.
*/
ButtonMonitor* button_monitor_alloc();

/**
 * @brief Frees a button monitor.
 * @param bm The button monitor to free.
*/
void button_monitor_free(ButtonMonitor* bm);

/**
 * @brief Sets the callback for the button monitor.
 * @param km The button monitor.
 * @param callback The callback to call when a button event occurs.
 * @param context The context for the callback.
*/
void button_monitor_set_callback(
    ButtonMonitor* km,
    ButtonMonitorEventCallback callback,
    void* context);

/**
 * @brief Gets the last button status.
 * @param km The button monitor.
 * @return The last button status.
*/
SwitchIds button_monitor_get_last_status(ButtonMonitor* km);