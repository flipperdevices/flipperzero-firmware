#include "app.h"
#include "app_config.h"
#include "app_keyboard_layout.h"

/*
This method handles Flipper D-Pad input when in the FlipboardKeyboard mode.
*/
bool flipboard_view_flip_keyboard_input(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

/*
This method handles drawing when in the FlipboardKeyboard mode.
*/
void flipboard_view_flip_keyboard_draw(Canvas* canvas, void* model) {
    static uint8_t counter = 0;

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

/*
This method handles FlipBoard key input when in the FlipboardKeyboard mode.
*/
bool flipboard_debounced_switch(void* context, uint8_t old_key, uint8_t new_key) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_key = flipboard_model_reduce(model, new_key, false);

    FURI_LOG_D(TAG, "SW EVENT: old=%d new=%d reduced=%d", old_key, new_key, reduced_new_key);

    KeySettingModel* ksm = flipboard_model_get_key_setting_model(model, reduced_new_key);
    flipboard_model_send_keystrokes(model, ksm);
    flipboard_model_send_text(model, ksm);
    flipboard_model_play_tone(model, ksm);
    flipboard_model_set_colors(model, ksm, new_key);

    return true;
}

/*
This method is invoked when entering the FlipboardKeyboard mode.
*/
void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_key_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_gui_refresh_speed_ms(fm, 100);
}

/*
This method is invoked when exiting the FlipboardKeyboard mode.
*/
void flipboard_exit_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_key_monitor(fm, NULL, NULL);
    flipboard_model_set_gui_refresh_speed_ms(fm, 0);
}

/*
This method configures the View* used for FlipboardKeyboard mode.
*/
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
    bool attach_keyboard = true;
    //bool attach_keyboard = false;

    Flipboard* app = flipboard_alloc(
        FLIPBOARD_APP_NAME,
        FLIPBOARD_PRIMARY_ITEM_NAME,
        ABOUT_TEXT,
        fields,
        single_mode_button,
        attach_keyboard,
        keys,
        shift_keys,
        COUNT_OF(keys) / 12,
        get_primary_view);
    view_dispatcher_run(flipboard_get_view_dispatcher(app));
    flipboard_free(app);

    return 0;
}
