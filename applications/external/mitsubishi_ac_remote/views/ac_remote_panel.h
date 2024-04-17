/**
 * @file ac_remote_panel.h
 * GUI: ACRemotePanel view module API
 */

#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Button panel module descriptor */
typedef struct ACRemotePanel ACRemotePanel;

typedef struct ButtonItem ButtonItem;

/** Callback type to call for handling selecting ac_remote_panel items */
typedef void (*ButtonItemCallback)(void* context, uint32_t index);

ACRemotePanel* ac_remote_panel_alloc(void);

void ac_remote_panel_free(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reset(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reset_selection(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reserve(ACRemotePanel* ac_remote_panel, size_t reserve_x, size_t reserve_y);

void ac_remote_panel_add_item(
    ACRemotePanel* ac_remote_panel,
    uint16_t index,
    // uint8_t current_value_index,
    // uint8_t values_count,
    uint16_t matrix_place_x,
    uint16_t matrix_place_y,
    uint16_t x,
    uint16_t y,
    const Icon* icon_name,
    const Icon* icon_name_selected,
    ButtonItemCallback callback,
    void* callback_context);

void ac_remote_panel_item_set_icons(
    ACRemotePanel* ac_remote_panel,
    uint32_t index,
    const Icon* icon_name,
    const Icon* icon_name_selected);

View* ac_remote_panel_get_view(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_add_label(
    ACRemotePanel* ac_remote_panel,
    int index,
    uint16_t x,
    uint16_t y,
    Font font,
    const char* label_str);

void ac_remote_panel_add_icon(
    ACRemotePanel* ac_remote_panel,
    uint16_t x,
    uint16_t y,
    const Icon* icon_name);

void ac_remote_panel_label_set_string(
    ACRemotePanel* ac_remote_panel,
    int index,
    const char* label_str);

#ifdef __cplusplus
}
#endif
