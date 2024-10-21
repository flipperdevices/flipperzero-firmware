#pragma once

#include <furi_hal.h>
#include <gui/view.h>

typedef struct ClockSettingsModule ClockSettingsModule;
typedef void (*ClockSettingsModuleViewCallback)(
    uint8_t channel_id,
    uint32_t freq,
    uint8_t duty,
    void* context);

ClockSettingsModule* clock_settings_module_alloc(FuriEventLoop* event_loop);

void clock_settings_module_free(ClockSettingsModule* instance);

View* clock_settings_module_get_view(ClockSettingsModule* instance);

void clock_settings_module_set(
    ClockSettingsModule* instance,
    const DateTime* datetime,
    bool enabled);

bool clock_settings_module_get(ClockSettingsModule* instance, DateTime* datetime);
