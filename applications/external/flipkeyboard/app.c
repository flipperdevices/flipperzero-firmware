#include "app.h"
#include "app_config.h"
#include "app_keyboard_layout.h"
// #include <assets_icons.h>

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
    FuriString* action_text = furi_string_alloc();
    FlipboardModelRef* my_model = (FlipboardModelRef*)model;
    ButtonMonitor* bm = flipboard_model_get_button_monitor(my_model->model);
    uint8_t action = 0;
    if(bm != NULL) {
        action = button_monitor_get_last_status(bm);
    }

    const Icon* icon1 = (action & 1) ? &I_fb_Down_hvr_25x27 : &I_fb_Up_25x27;
    const Icon* icon2 = (action & 2) ? &I_fb_Down_hvr_25x27 : &I_fb_Up_25x27;
    const Icon* icon3 = (action & 4) ? &I_fb_Down_hvr_25x27 : &I_fb_Up_25x27;
    const Icon* icon4 = (action & 8) ? &I_fb_Down_hvr_25x27 : &I_fb_Up_25x27;
    furi_string_printf(action_text, "%02d", action);

    canvas_set_bitmap_mode(canvas, 1);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 21, 12, "PRESS BUTTONS");

    canvas_draw_icon(canvas, 5, 19, icon1);
    canvas_draw_icon(canvas, 36, 19, icon2);
    canvas_draw_icon(canvas, 67, 19, icon3);
    canvas_draw_icon(canvas, 98, 19, icon4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 33, 60, "ACTION:");
    canvas_draw_str(canvas, 84, 60, furi_string_get_cstr(action_text));

    furi_string_free(action_text);
}

/**
 * @brief This method handles FlipBoard button input when in the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 * @param old_key The previous key state.
 * @param new_key The new key state.
 */
void flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

    FURI_LOG_D(
        TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);

    flipboard_model_update_gui(model);

    ButtonModel* bm = flipboard_model_get_button_model(model, reduced_new_button);
    flipboard_model_set_colors(model, bm, new_button);
    flipboard_model_send_keystrokes(model, bm);
    flipboard_model_send_text(model, bm);
    flipboard_model_play_tone(model, bm);
}

/**
 * @brief This method is invoked when entering the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_button_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_gui_refresh_speed_ms(fm, 0);
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
 * @brief This method is invoked when the app menu is loaded.
 * @details This method is invoked when the app menu is loaded.  It is used to
 *         display a startup animation.
 * @param model The FlipboardModel* context.
 */
static void loaded_app_menu(FlipboardModel* model) {
    static bool initial_load = true;
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    UNUSED(color_names);
    UNUSED(color_values);
    if(initial_load) {
        for(int i = 0; i < 7; i++) {
            flipboard_leds_set(leds, LedId1, (1 << (16 + i)));
            flipboard_leds_set(leds, LedId2, (1 << (0 + i)));
            flipboard_leds_set(leds, LedId3, (1 << (8 + i)));
            flipboard_leds_set(leds, LedId4, (1 << (0 + i)) | (1 << (8 + i)));
            flipboard_leds_update(leds);
            furi_delay_ms(100);
        }
        for(int i = 7; i > 0; i--) {
            flipboard_leds_set(leds, LedId1, (1 << (16 + i)));
            flipboard_leds_set(leds, LedId2, (1 << (0 + i)));
            flipboard_leds_set(leds, LedId3, (1 << (8 + i)));
            flipboard_leds_set(leds, LedId4, (1 << (0 + i)) | (1 << (8 + i)));
            flipboard_leds_update(leds);
            furi_delay_ms(100);
        }
        initial_load = false;
    }

    flipboard_leds_reset(leds);
    flipboard_leds_update(leds);
}

/**
 * @brief This method is invoked when a custom event is received.
 * @param context The Flipboard* context.
 * @param event The event to handle.
 * @return true if the event was handled, false otherwise.
 */
static bool custom_event_handler(void* context, uint32_t event) {
    FlipboardModel* model = flipboard_get_model((Flipboard*)context);

    if(event == CustomEventAppMenuEnter) {
        loaded_app_menu(model);
    }

    return true;
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
    // attach_keyboard = false;

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
    view_dispatcher_set_event_callback_context(flipboard_get_view_dispatcher(app), app);
    view_dispatcher_set_custom_event_callback(
        flipboard_get_view_dispatcher(app), custom_event_handler);
    view_dispatcher_run(flipboard_get_view_dispatcher(app));
    flipboard_free(app);

    return 0;
}
