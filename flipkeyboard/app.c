#include "app.h"
#include "app_config.h"
#include "app_keyboard_layout.h"

/**
 * @brief This method handles Flipper D-Pad input when in the FlipboardKeyboard mode.
 * @param event The InputEvent* to handle.
 * @param context The Flipboard* context.
 * @return true if the key event was handled, false otherwise.
 */
bool flipboard_view_flip_keyboard_input(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

/**
 * @brief This method handles drawing when in the FlipboardKeyboard mode.
 * @param canvas The canvas to draw on.
 * @param model The FlipboardModelRef* context.
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

    ButtonMonitor* bm = flipboard_model_get_button_monitor(my_model->model);
    if(bm != NULL) {
        uint8_t last = button_monitor_get_last_status(bm);
        FuriString* str = furi_string_alloc();
        furi_string_printf(str, "%02X   %02x", last, counter++);
        canvas_draw_str(canvas, 55, 50, furi_string_get_cstr(str));
        furi_string_free(str);
    }

    // canvas_draw_icon(canvas, 64, 42, &I_glyph_1_7x9);
}

/**
 * @brief This method handles FlipBoard button input when in the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 * @param old_key The previous key state.
 * @param new_key The new key state.
 * @return true if the key event was handled, false otherwise.
 */
bool flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

    FURI_LOG_D(
        TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);

    ButtonModel* bsm = flipboard_model_get_button_model(model, reduced_new_button);
    flipboard_model_set_colors(model, bsm, new_button);
    flipboard_model_send_keystrokes(model, bsm);
    flipboard_model_send_text(model, bsm);
    flipboard_model_play_tone(model, bsm);

    return true;
}

/**
 * @brief This method is invoked when entering the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_button_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_gui_refresh_speed_ms(fm, 100);
}

/**
 * @brief This method is invoked when exiting the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
void flipboard_exit_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_button_monitor(fm, NULL, NULL);
    flipboard_model_set_gui_refresh_speed_ms(fm, 0);
}

/**
 * @brief This method configures the View* used for FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 * @return View* for FlipboardKeyboard mode.
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

/**
 * @brief This method is invoked when the FlipboardKeyboard app is launched.
 * @param p Unused.
 * @return 0.
 */
int32_t flipboard_keyboard_app(void* p) {
    UNUSED(p);

    ButtonModelFields fields = ButtonModelFieldAll;
    bool single_mode_button = false;
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
