#pragma once

#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/flipper_format/flipper_format.h>

typedef struct {
    bool ext_5v;
    bool ext_out;
    bool auto_detect;

    bool _otg_was_enabled;
} InfraredLastSettings;

InfraredLastSettings* infrared_last_settings_alloc(void);
void infrared_last_settings_free(InfraredLastSettings* instance);
void infrared_last_settings_load(InfraredLastSettings* instance);
bool infrared_last_settings_save(InfraredLastSettings* instance);
void infrared_last_settings_apply(InfraredLastSettings* instance);
void infrared_last_settings_reset(InfraredLastSettings* instance);