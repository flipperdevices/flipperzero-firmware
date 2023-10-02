#include "flipboard_model_i.h"

FlipboardModel*
    flipboard_model_alloc(char* app_name, bool single_button_mode, KeySettingModelFields fields) {
    FlipboardModel* model = (FlipboardModel*)malloc(sizeof(FlipboardModel));
    model->name = app_name;
    model->key_setting_model_fields = fields;
    model->single_button_mode = single_button_mode;
    model->keyboard = flipboard_keyboard_alloc();
    model->speaker = speaker_alloc();
    model->key_monitor = NULL;
    model->gui_refresh_timer = NULL;
    model->leds = flipboard_leds_alloc();
    model->backlight_always_on = true;
    if(model->backlight_always_on) {
        backlight_on();
    }
    flipboard_leds_reset(model->leds);

    flipboard_load(model);

    return model;
}

char* flipboard_model_get_name(FlipboardModel* model) {
    return model->name;
}

bool flipboard_model_get_single_button_mode(FlipboardModel* model) {
    return model->single_button_mode;
}

KeySettingModelFields flipboard_model_get_key_setting_model_fields(FlipboardModel* model) {
    return model->key_setting_model_fields;
}

KeySettingModel* flipboard_model_get_key_setting_model(FlipboardModel* model, uint8_t key) {
    return model->key_setting_model[key];
}

KeyMonitor* flipboard_model_get_key_monitor(FlipboardModel* model) {
    return model->key_monitor;
}

FlipboardKeyboard* flipboard_model_get_keyboard(FlipboardModel* model) {
    return model->keyboard;
}

Speaker* flipboard_model_get_speaker(FlipboardModel* model) {
    return model->speaker;
}

FlipboardLeds* flipboard_model_get_leds(FlipboardModel* model) {
    return model->leds;
}

static void __gui_redraw() {
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_direct_draw_acquire(gui);
    gui_direct_draw_release(gui);
}

static void tick(void* context) {
    UNUSED(context);
    __gui_redraw();
}

void flipboard_model_set_gui_refresh_speed_ms(FlipboardModel* model, uint32_t update_rate_ms) {
    if(update_rate_ms == 0) {
        if(model->gui_refresh_timer) {
            furi_timer_free(model->gui_refresh_timer);
            model->gui_refresh_timer = NULL;
        }
        return;
    }

    if(model->gui_refresh_timer == NULL) {
        model->gui_refresh_timer = furi_timer_alloc(tick, FuriTimerTypePeriodic, model);
    }
    furi_timer_start(model->gui_refresh_timer, furi_ms_to_ticks(update_rate_ms));
}

void flipboard_model_set_key_setting_model(
    FlipboardModel* model,
    uint8_t key,
    KeySettingModel* key_setting_model) {
    model->key_setting_model[key] = key_setting_model;
}

void flipboard_model_set_key_monitor(
    FlipboardModel* model,
    KeyMonitorEventCallback callback,
    void* context) {
    if(model->key_monitor) {
        key_monitor_free(model->key_monitor);
        model->key_monitor = NULL;
    }

    if(callback != NULL) {
        model->key_monitor = key_monitor_alloc();
        key_monitor_set_callback(model->key_monitor, callback, context);
    }
}

void flipboard_model_free(FlipboardModel* model) {
    flipboard_save(model, model->key_setting_model_fields);

    if(model->speaker) {
        speaker_free(model->speaker);
    }

    if(model->backlight_always_on) {
        backlight_off();
    }

    if(model->key_monitor) {
        key_monitor_free(model->key_monitor);
    }

    if(model->keyboard) {
        flipboard_keyboard_free(model->keyboard);
    }

    if(model->leds) {
        flipboard_leds_free(model->leds);
    }

    if(model->gui_refresh_timer) {
        furi_timer_free(model->gui_refresh_timer);
    }

    free(model);
}

void flipboard_model_play_tone(FlipboardModel* model, KeySettingModel* ksm) {
    Speaker* speaker = flipboard_model_get_speaker(model);
    speaker_set_frequency(speaker, key_setting_model_get_frequency(ksm));
}

void flipboard_model_set_colors(FlipboardModel* model, KeySettingModel* ksm, uint8_t new_key) {
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    uint32_t color = ksm ? key_setting_model_get_color_down(ksm) : 0xFFFFFF;
    KeySettingModel* ksm1 = flipboard_model_get_key_setting_model(model, SwitchId1);
    KeySettingModel* ksm2 = flipboard_model_get_key_setting_model(model, SwitchId2);
    KeySettingModel* ksm3 = flipboard_model_get_key_setting_model(model, SwitchId3);
    KeySettingModel* ksm4 = flipboard_model_get_key_setting_model(model, SwitchId4);
    uint32_t color1 = ksm1 ? key_setting_model_get_color_up(ksm1) : 0x000000;
    uint32_t color2 = ksm2 ? key_setting_model_get_color_up(ksm2) : 0x000000;
    uint32_t color3 = ksm3 ? key_setting_model_get_color_up(ksm3) : 0x000000;
    uint32_t color4 = ksm4 ? key_setting_model_get_color_up(ksm4) : 0x000000;
    color1 = (new_key & LedId1) ? color : color1;
    color2 = (new_key & LedId2) ? color : color2;
    color3 = (new_key & LedId3) ? color : color3;
    color4 = (new_key & LedId4) ? color : color4;
    flipboard_leds_set(leds, LedId1, color1);
    flipboard_leds_set(leds, LedId2, color2);
    flipboard_leds_set(leds, LedId3, color3);
    flipboard_leds_set(leds, LedId4, color4);
    flipboard_leds_update(leds);
}

void flipboard_model_send_keystrokes(FlipboardModel* model, KeySettingModel* ksm) {
    uint8_t keystroke_count = key_setting_model_get_keystrokes_count(ksm);
    uint16_t modifiers = 0;
    for(int i = 0; i < keystroke_count; i++) {
        Keystroke keystroke = key_setting_model_get_keystroke(ksm, i);
        if(keystroke.key_code == 0 || keystroke.count == 0) {
            continue;
        }

        if(keystroke.key_code == 1) {
            if(keystroke.count > 0) {
                furi_delay_ms(100 * keystroke.count);
            }

            modifiers = 0;
            continue;
        }

        uint16_t send_modifiers = 0;
        if(keystroke.key_code == HID_KEYBOARD_L_CTRL) {
            send_modifiers = (1 << 8);
        } else if(keystroke.key_code == HID_KEYBOARD_L_SHIFT) {
            send_modifiers = (1 << 9);
        } else if(keystroke.key_code == HID_KEYBOARD_L_ALT) {
            send_modifiers = (1 << 10);
        } else if(keystroke.key_code == HID_KEYBOARD_L_GUI) {
            send_modifiers = (1 << 11);
        } else if(keystroke.key_code == HID_KEYBOARD_R_CTRL) {
            send_modifiers = (1 << 12);
        } else if(keystroke.key_code == HID_KEYBOARD_R_SHIFT) {
            send_modifiers = (1 << 13);
        } else if(keystroke.key_code == HID_KEYBOARD_R_ALT) {
            send_modifiers = (1 << 14);
        } else if(keystroke.key_code == HID_KEYBOARD_R_GUI) {
            send_modifiers = (1 << 15);
        }

        modifiers |= send_modifiers;

        for(uint8_t count = keystroke.count; count != 0; count--) {
            flipboard_keyboard_send_keycode(
                flipboard_model_get_keyboard(model), keystroke.key_code | modifiers);
            flipboard_keyboard_release_all(flipboard_model_get_keyboard(model));
        }

        if(!send_modifiers) {
            modifiers = 0;
        }
    }
}

void flipboard_model_send_text(FlipboardModel* model, KeySettingModel* ksm) {
    FuriString* message = key_setting_model_get_message(ksm);
    if(message) {
        flipboard_keyboard_send_text(
            flipboard_model_get_keyboard(model), furi_string_get_cstr(message));
    }
}

uint8_t flipboard_model_reduce(FlipboardModel* model, uint8_t new_key, bool left_wins) {
    uint8_t reduced_new_key = new_key;

    if(new_key == 0) {
        return 0;
    }

    if(flipboard_model_get_single_button_mode(model)) {
        uint8_t mask = left_wins ? 0x1 : 0x80;
        while(((new_key & mask) == 0) && mask) {
            if(left_wins) {
                mask = mask << 1;
            } else {
                mask = mask >> 1;
            }
        }
        reduced_new_key = mask;
    }

    return reduced_new_key;
}