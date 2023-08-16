/*
    RGB backlight FlipperZero driver
    Copyright (C) 2022-2023 Victor Nikitchuk (https://github.com/quen0n)
    Heavily modified by Willy-JL and Z3bro

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "rgb_backlight.h"
#include <furi_hal.h>
#include <storage/storage.h>
#include <toolbox/saved_struct.h>

#define RGB_BACKLIGHT_SETTINGS_MAGIC 0x15
#define RGB_BACKLIGHT_SETTINGS_VERSION 6
#define RGB_BACKLIGHT_SETTINGS_PATH CFG_PATH("rgb_backlight.settings")

static struct {
    RgbColor colors[SK6805_LED_COUNT];
    RGBBacklightRainbowMode rainbow_mode;
    uint8_t rainbow_speed;
    uint32_t rainbow_interval;
    uint32_t rainbow_saturation;
} rgb_settings = {
    .colors =
        {
            {255, 69, 0},
            {255, 69, 0},
            {255, 69, 0},
        },
    .rainbow_mode = RGBBacklightRainbowModeOff,
    .rainbow_speed = 5,
    .rainbow_interval = 250,
    .rainbow_saturation = 255,
};

static struct {
    bool settings_loaded;
    bool enabled;
    bool last_rainbow;
    uint8_t last_brightness;
    RgbColor last_colors[SK6805_LED_COUNT];
    FuriTimer* rainbow_timer;
    HsvColor rainbow_hsv;
} rgb_state = {
    .settings_loaded = false,
    .enabled = false,
    .last_rainbow = true,
    .last_brightness = 0,
    .last_colors =
        {
            {0, 0, 0},
            {0, 0, 0},
            {0, 0, 0},
        },
    .rainbow_timer = NULL,
    .rainbow_hsv = {0, 255, 255},
};

static void rainbow_timer(void* ctx) {
    UNUSED(ctx);
    rgb_backlight_update(rgb_state.last_brightness, true);
}

void rgb_backlight_reconfigure(bool enabled) {
    if(enabled && !rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_state.enabled = enabled;

    if(rgb_state.enabled && rgb_settings.rainbow_mode != RGBBacklightRainbowModeOff) {
        if(rgb_state.rainbow_timer == NULL) {
            rgb_state.rainbow_timer = furi_timer_alloc(rainbow_timer, FuriTimerTypePeriodic, NULL);
        } else {
            furi_timer_stop(rgb_state.rainbow_timer);
        }
        furi_timer_start(rgb_state.rainbow_timer, rgb_settings.rainbow_interval);
    } else if(rgb_state.rainbow_timer != NULL) {
        furi_timer_stop(rgb_state.rainbow_timer);
        furi_timer_free(rgb_state.rainbow_timer);
        rgb_state.rainbow_timer = NULL;
    }
    rgb_state.rainbow_hsv.s = rgb_settings.rainbow_saturation;

    rgb_backlight_update(rgb_state.last_brightness, false);
}

void rgb_backlight_load_settings(void) {
    // Do not load data from internal memory when booting in DFU mode
    if(!furi_hal_is_normal_boot() || rgb_state.settings_loaded) {
        rgb_state.settings_loaded = true;
        return;
    }

    saved_struct_load(
        RGB_BACKLIGHT_SETTINGS_PATH,
        &rgb_settings,
        sizeof(rgb_settings),
        RGB_BACKLIGHT_SETTINGS_MAGIC,
        RGB_BACKLIGHT_SETTINGS_VERSION);

    rgb_state.settings_loaded = true;
    rgb_backlight_reconfigure(rgb_state.enabled);
}

void rgb_backlight_save_settings(void) {
    saved_struct_save(
        RGB_BACKLIGHT_SETTINGS_PATH,
        &rgb_settings,
        sizeof(rgb_settings),
        RGB_BACKLIGHT_SETTINGS_MAGIC,
        RGB_BACKLIGHT_SETTINGS_VERSION);
}

void rgb_backlight_set_color(uint8_t index, RgbColor color) {
    if(index >= COUNT_OF(rgb_settings.colors)) return;
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.colors[index] = color;
    rgb_backlight_reconfigure(rgb_state.enabled);
}

RgbColor rgb_backlight_get_color(uint8_t index) {
    if(index >= COUNT_OF(rgb_settings.colors)) return (RgbColor){0, 0, 0};
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.colors[index];
}

void rgb_backlight_set_rainbow_mode(RGBBacklightRainbowMode rainbow_mode) {
    if(rainbow_mode >= RGBBacklightRainbowModeCount) return;
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_mode = rainbow_mode;
    rgb_backlight_reconfigure(rgb_state.enabled);
}

RGBBacklightRainbowMode rgb_backlight_get_rainbow_mode() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_mode;
}

void rgb_backlight_set_rainbow_speed(uint8_t rainbow_speed) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_speed = rainbow_speed;
}

uint8_t rgb_backlight_get_rainbow_speed() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_speed;
}

void rgb_backlight_set_rainbow_interval(uint32_t rainbow_interval) {
    if(rainbow_interval < 100) return;
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_interval = rainbow_interval;
    rgb_backlight_reconfigure(rgb_state.enabled);
}

uint32_t rgb_backlight_get_rainbow_interval() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_interval;
}

void rgb_backlight_set_rainbow_saturation(uint8_t rainbow_saturation) {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    rgb_settings.rainbow_saturation = rainbow_saturation;
    rgb_backlight_reconfigure(rgb_state.enabled);
}

uint8_t rgb_backlight_get_rainbow_saturation() {
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }
    return rgb_settings.rainbow_saturation;
}

void rgb_backlight_update(uint8_t brightness, bool tick) {
    if(!rgb_state.enabled) return;
    if(!rgb_state.settings_loaded) {
        rgb_backlight_load_settings();
    }

    switch(rgb_settings.rainbow_mode) {
    case RGBBacklightRainbowModeOff: {
        if(!rgb_state.last_rainbow && rgb_state.last_brightness == brightness &&
           memcmp(rgb_state.last_colors, rgb_settings.colors, sizeof(rgb_settings.colors)) == 0) {
            return;
        }
        rgb_state.last_rainbow = false;
        memcpy(rgb_state.last_colors, rgb_settings.colors, sizeof(rgb_settings.colors));

        float bright = brightness / 255.0f;
        for(uint8_t i = 0; i < SK6805_get_led_count(); i++) {
            SK6805_set_led_color(
                i,
                rgb_settings.colors[i].r * bright,
                rgb_settings.colors[i].g * bright,
                rgb_settings.colors[i].b * bright);
        }
        break;
    }

    case RGBBacklightRainbowModeWave:
    case RGBBacklightRainbowModeSolid: {
        rgb_state.last_rainbow = true;

        if(tick && brightness) {
            rgb_state.rainbow_hsv.h += rgb_settings.rainbow_speed;
        } else {
            if(rgb_state.last_brightness == brightness && rgb_state.last_rainbow) {
                return;
            }
            rgb_state.rainbow_hsv.v = brightness;
        }

        HsvColor hsv = rgb_state.rainbow_hsv;
        RgbColor rgb = hsv2rgb(hsv);

        for(uint8_t i = 0; i < SK6805_get_led_count(); i++) {
            if(i && rgb_settings.rainbow_mode == RGBBacklightRainbowModeWave) {
                hsv.h += (50 * i);
                rgb = hsv2rgb(hsv);
            }
            SK6805_set_led_color(i, rgb.r, rgb.g, rgb.b);
        }
        break;
    }

    default:
        return;
    }

    rgb_state.last_brightness = brightness;
    SK6805_update();
}
