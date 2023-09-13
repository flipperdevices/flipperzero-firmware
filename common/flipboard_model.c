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