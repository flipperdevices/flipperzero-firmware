#pragma once

#include <gui/view.h>
#include "../helpers/subghz_custom_event.h"

typedef struct SubghzViewTransmitter SubghzViewTransmitter;

typedef void (*SubghzViewTransmitterCallback)(SubghzCustomEvent event, void* context);

void subghz_view_transmitter_set_callback(
    SubghzViewTransmitter* subghz_transmitter,
    SubghzViewTransmitterCallback callback,
    void* context);

SubghzViewTransmitter* subghz_view_transmitter_alloc();

void subghz_view_transmitter_free(SubghzViewTransmitter* subghz_transmitter);

View* subghz_view_transmitter_get_view(SubghzViewTransmitter* subghz_transmitter);

void subghz_view_transmitter_add_data_to_show(
    SubghzViewTransmitter* subghz_transmitter,
    const char* key_str,
    const char* frequency_str,
    const char* preset_str,
    uint8_t show_button);
