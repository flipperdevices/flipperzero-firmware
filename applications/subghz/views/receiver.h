#pragma once

#include <gui/view.h>
#include "../helpers/subghz_custom_event.h"

typedef struct SubghzViewReceiver SubghzViewReceiver;

typedef void (*SubghzViewReceiverCallback)(SubghzCustomEvent event, void* context);

void subghz_view_receiver_set_callback(
    SubghzViewReceiver* subghz_receiver,
    SubghzViewReceiverCallback callback,
    void* context);

SubghzViewReceiver* subghz_view_receiver_alloc();

void subghz_view_receiver_free(SubghzViewReceiver* subghz_receiver);

View* subghz_view_receiver_get_view(SubghzViewReceiver* subghz_receiver);

void subghz_view_receiver_add_data_statusbar(
    SubghzViewReceiver* subghz_receiver,
    const char* frequency_str,
    const char* preset_str,
    const char* history_stat_str);

void subghz_view_receiver_add_item_to_menu(
    SubghzViewReceiver* subghz_receiver,
    const char* name,
    uint8_t type);

uint16_t subghz_view_receiver_get_idx_menu(SubghzViewReceiver* subghz_receiver);

void subghz_view_receiver_set_idx_menu(SubghzViewReceiver* subghz_receiver, uint16_t idx);

void subghz_view_receiver_exit(void* context);
