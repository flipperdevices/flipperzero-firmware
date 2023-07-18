/**
 * @file ac_remote_panel.h
 * GUI: ACRemotePanel view module API
 */

#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ACRemotePanel ACRemotePanel;

typedef struct VariableButtonItem VariableButtonItem;
typedef void (*VariableButtonItemCallback)(VariableButtonItem* item);

ACRemotePanel* ac_remote_panel_alloc(void);

void ac_remote_panel_free(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reset(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reset_selection(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_reserve(ACRemotePanel* ac_remote_panel, size_t reserve_x, size_t reserve_y);

void ac_remote_panel_add_item(
    ACRemotePanel* ac_remote_panel,
    uint16_t id,
    uint8_t current_value_index,
    uint8_t values_count,
    uint16_t matrix_place_x,
    uint16_t matrix_place_y,
    uint16_t x,
    uint16_t y,
    const Icon* icon_name,
    const Icon* icon_name_selected,
    VariableButtonItemCallback callback,
    void* callback_context);

uint16_t ac_remote_panel_item_get_index(VariableButtonItem* item);

uint8_t ac_remote_panel_item_get_current_value_index(VariableButtonItem* item);

void* ac_remote_panel_item_get_context(VariableButtonItem* item);

void ac_remote_panel_item_set_icon_name(VariableButtonItem* item, const Icon* icon_name, const Icon* icon_name_selected);

View* ac_remote_panel_get_view(ACRemotePanel* ac_remote_panel);

void ac_remote_panel_add_label(
    ACRemotePanel* ac_remote_panel,
    int index,
    uint16_t x,
    uint16_t y,
    Font font,
    const char* label_str);

void ac_remote_panel_label_set_string(ACRemotePanel* ac_remote_panel, int index, const char* label_str);

#ifdef __cplusplus
}
#endif
