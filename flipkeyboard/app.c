#include "app_i.h"

static void play_tone(FlipboardModel* model, KeySettingModel* ksm);
static void set_colors(FlipboardModel* model, KeySettingModel* ksm, uint8_t new_key);
static void send_keystrokes(FlipboardModel* model, KeySettingModel* ksm);
static uint8_t reduce(uint8_t new_key, bool left_wins);

// -----------------------------------------------------------
//            THIS IS YOUR MAIN APPLICATION CODE
// -----------------------------------------------------------

bool flipboard_view_flip_keyboard_input(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

uint8_t counter = 0;
void flipboard_view_flip_keyboard_draw(Canvas* canvas, void* model) {
    FlipboardModelRef* my_model = (FlipboardModelRef*)model;
    canvas_draw_str(canvas, 2, 15, "PRESS FLIPBOARD");

    if(flipboard_model_get_single_button_mode(my_model->model)) {
        canvas_draw_str(canvas, 22, 30, "BUTTON");
    } else {
        canvas_draw_str(canvas, 22, 30, "BUTTONS");
    }

    KeyMonitor* km = flipboard_model_get_key_monitor(my_model->model);
    if(km != NULL) {
        uint8_t last = key_monitor_get_last_status(km);
        FuriString* str = furi_string_alloc();
        furi_string_printf(str, "%02X   %02x", last, counter++);
        canvas_draw_str(canvas, 55, 50, furi_string_get_cstr(str));
        furi_string_free(str);
    }

    // canvas_draw_icon(canvas, 64, 42, &I_glyph_1_7x9);
}

bool flipboard_debounced_switch(void* context, uint8_t old_key, uint8_t new_key) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_key = new_key;

    if(flipboard_model_get_single_button_mode(model)) {
        reduced_new_key = reduce(new_key, false);
    }

    FURI_LOG_D(TAG, "SW EVENT: old=%d new=%d reduced=%d", old_key, new_key, reduced_new_key);

    KeySettingModel* ksm = flipboard_model_get_key_setting_model(model, reduced_new_key);
    send_keystrokes(model, ksm);
    play_tone(model, ksm);
    set_colors(model, ksm, new_key);

    return true;
}

void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_key_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    set_colors(fm, NULL, 0x0);
    flipboard_model_set_gui_refresh_speed_ms(fm, 100);
}

void flipboard_exit_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    set_colors(fm, NULL, 0x0);
    flipboard_model_set_key_monitor(fm, NULL, NULL);
    flipboard_model_set_gui_refresh_speed_ms(fm, 0);
}

// -----------------------------------------------------------
//            BELOW HERE ARE JUST HELPER METHODS
// -----------------------------------------------------------

View* get_primary_view(void* context) {
    FlipboardModel* model = flipboard_get_model((Flipboard*)context);
    View* view_primary = view_alloc();
    view_set_draw_callback(view_primary, flipboard_view_flip_keyboard_draw);
    view_set_input_callback(view_primary, flipboard_view_flip_keyboard_input);
    view_set_previous_callback(view_primary, flipboard_navigation_show_app_menu);
    view_set_enter_callback(view_primary, flipboard_enter_callback);
    view_set_exit_callback(view_primary, flipboard_exit_callback);
    view_set_context(view_primary, context);
    view_allocate_model(view_primary, ViewModelTypeLockFree, sizeof(FlipboardModelRef));
    FlipboardModelRef* ref = (FlipboardModelRef*)view_get_model(view_primary);
    ref->model = model;
    return view_primary;
}

int32_t flipboard_keyboard_app(void* p) {
    UNUSED(p);

    KeySettingModelFields fields = KeySettingModelFieldAll;
    bool single_mode_button = true;
    bool attach_keyboard = false;

    Flipboard* app =
        flipboard_alloc(fields, single_mode_button, attach_keyboard, get_primary_view);
    view_dispatcher_run(flipboard_get_view_dispatcher(app));
    flipboard_free(app);

    return 0;
}

static void play_tone(FlipboardModel* model, KeySettingModel* ksm) {
    Speaker* speaker = flipboard_model_get_speaker(model);
    speaker_set_frequency(speaker, key_setting_model_get_frequency(ksm));
}

static void set_colors(FlipboardModel* model, KeySettingModel* ksm, uint8_t new_key) {
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

static void send_keystrokes(FlipboardModel* model, KeySettingModel* ksm) {
    uint8_t keystroke_count = key_setting_model_get_keystrokes_count(ksm);
    for(int i = 0; i < keystroke_count; i++) {
        Keystroke keystroke = key_setting_model_get_keystroke(ksm, i);
        if(keystroke.key_code == 0 || keystroke.count == 0) {
            continue;
        }

        if(keystroke.key_code == 0) {
            continue;
        }

        for(uint8_t count = keystroke.count; count != 0; count--) {
            flipboard_keyboard_send_keycode(
                flipboard_model_get_keyboard(model), keystroke.key_code);
            flipboard_keyboard_release_all(flipboard_model_get_keyboard(model));
        }
    }
}

static uint8_t reduce(uint8_t new_key, bool left_wins) {
    if(new_key == 0) {
        return 0;
    }

    uint8_t mask = left_wins ? 0x1 : 0x80;
    while(((new_key & mask) == 0) && mask) {
        if(left_wins) {
            mask = mask << 1;
        } else {
            mask = mask >> 1;
        }
    }

    return mask;
}
