#pragma once

#include <furi.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include "rolling_flaws_subghz_receive.h"

typedef struct {
    uint8_t protocol_index;
    FuriString* custom_mf;
    uint8_t frequency_index;
    uint8_t replay_index; // allow replay attack?
    uint8_t window_next_index; // how many codes forward are acceptable?
    uint8_t window_future_index; // how many codes forward are considered future?
    uint8_t window_future_gap_index; // how far can two sequential future codes be?
    uint8_t fix_index; // SN+button
    uint32_t custom_fix; // Fix value if custom is selected.
    uint8_t sn_zero_index; // allow decoded 00 to wildcard SN.
    uint8_t sn_bits_index; // number of bits to compare. (custom fw only?)
    uint8_t count_zero_index; // allow count of 0 to be considered an open.

    uint32_t count; // 0 to 0xFFFF
    uint32_t future_count; // 0 to 0xFFFF, use 0xFFFFFFFF if cleared.
    bool opened;

    FuriString* key;
    FuriString* status;
    VariableItem* variable_item_protocol;
    VariableItem* variable_item_fix;
} RollingFlawsModel;

typedef struct {
    RollingFlawsModel* model;
} RollingFlawsRefModel;

typedef struct {
    NotificationApp* notifications;

    ViewDispatcher* view_dispatcher;

    Submenu* submenu;
    VariableItemList* variable_item_list_config;
    View* view_receive_signals;
    View* view_receive_sync;
    Widget* widget_about;

    RollingFlawsModel* model;
    RollingFlawsSubGhz* subghz;
} RollingFlaws;