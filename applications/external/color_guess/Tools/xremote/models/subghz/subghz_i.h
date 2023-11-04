
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include <flipper_format/flipper_format.h>
#include <lib/subghz/protocols/raw.h>
//#include <lib/toolbox/path.h>
//#include <flipper_format/flipper_format_i.h>
#include <lib/toolbox/stream/stream.h>
//#include <lib/subghz/protocols/protocol_items.h> //Not found
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/devices/devices.h>
//#include <lib/subghz/blocks/custom_btn.h>

#include <flipper_format/flipper_format_i.h>

#include "subghz_types.h"

extern const SubGhzProtocolRegistry subghz_protocol_registry;

struct SubGhzTxRx {
    //SubGhzWorker* worker;

    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    //SubGhzTransmitter* transmitter;
    //SubGhzProtocolFlag filter;
    SubGhzProtocolDecoderBase* decoder_result;
    FlipperFormat* fff_data;

    SubGhzRadioPreset* preset;
    //SubGhzHistory* history;
    //uint16_t idx_menu_chosen;
    //SubGhzTxRxState txrx_state;
    //SubGhzHopperState hopper_state;
    //SubGhzSpeakerState speaker_state;
    //uint8_t hopper_timeout;
    //uint8_t hopper_idx_frequency;
    //SubGhzRxKeyState rx_key_state;

    //float raw_threshold_rssi;
    //uint8_t raw_threshold_rssi_low_count;
    const SubGhzDevice* radio_device;
};

typedef struct SubGhzTxRx SubGhzTxRx;
