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
    FuriString* action_text = furi_string_alloc();
    FlipboardModelRef* my_model = (FlipboardModelRef*)model;
    FlipboardKeyboard* keyboard = flipboard_model_get_keyboard(my_model->model);
    FlipboardKeyboardInterface interface = flipboard_keyboard_get_inteface(keyboard);
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
    if(interface == FlipboardKeyboardInterfaceUsb) {
        canvas_draw_str(canvas, 6, 12, "FlipKeyboard (USB)");
    } else if(interface == FlipboardKeyboardInterfaceBle) {
        canvas_draw_str(canvas, 6, 12, "FlipKeyboard (BLE)");
    } else {
        canvas_draw_str(canvas, 6, 12, "FlipKeyboard (USB/BLE)");
    }

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

    ActionModel* action_model = flipboard_model_get_action_model(model, reduced_new_button);
    flipboard_model_set_colors(model, action_model, new_button);
#ifdef FLIPBOARD_KEYBOARD_SEND_MESSAGES_AFTER_KEYSTROKES
    if(!flipboard_model_send_keystrokes(model, action_model)) {
        //  If keystrokes did not send any messages, then we will send them in order.
        for(int i = 0; i < 4; i++) {
            flipboard_model_send_text(model, action_model, i);
        }
    }
#else
    if(action_model_get_keystrokes_count(action_model) > 0) {
        flipboard_model_send_keystrokes(model, action_model);
    } else {
        for(int i = 0; i < 4; i++) {
            flipboard_model_send_text(model, action_model, i);
        }
    }
#endif
    flipboard_model_play_tone(model, action_model);
}

/**
 * @brief This method is invoked when entering the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_keyboard_attach(flipboard_model_get_keyboard(fm));
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
    flipboard_keyboard_detatch(flipboard_model_get_keyboard(fm));
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

static void flipboard_defaults(FlipboardModel* model) {
    ActionModel* action_model;
    action_model = action_model_alloc(1);
    action_model_set_color_up(action_model, LedColorBlue);
    action_model_set_color_down(action_model, LedColorCyan);
    action_model_set_frequency(action_model, 164.814);
    action_model_append_keystroke(action_model, 0xF1, 1); // Msg 1
    action_model_set_message(action_model, "Welcome to FlipKeyboard!", 0);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(2);
    action_model_set_color_up(action_model, LedColorRed);
    action_model_set_color_down(action_model, LedColorMagenta);
    action_model_set_frequency(action_model, 220);
    action_model_append_keystroke(action_model, 0xF1, 1); // Msg 1
    action_model_set_message(action_model, "https://discord.com/invite/NsjCvqwPAd", 0);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(4);
    action_model_set_color_up(action_model, LedColorGreen);
    action_model_set_color_down(action_model, LedColorViolet);
    action_model_set_frequency(action_model, 277.183);
    action_model_append_keystroke(action_model, 0xF1, 1); // Msg 1
    action_model_set_message(action_model, "https://youtube.com/@MrDerekJamison", 0);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(8);
    action_model_set_color_up(action_model, LedColorOrange);
    action_model_set_color_down(action_model, LedColorYellow);
    action_model_set_frequency(action_model, 329.628);
    action_model_append_keystroke(action_model, 0xF1, 1); // Msg 1
    action_model_set_message(action_model, "https://youtube.com/@MakeItHackin", 0);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
}

/**
 * @brief This method is invoked when the FlipboardKeyboard app is launched.
 * @param p Unused.
 * @return 0.
 */
int32_t flipboard_keyboard_app(void* p) {
    UNUSED(p);

    ActionModelFields fields = ActionModelFieldAll;
    bool single_mode_button = false;

    Flipboard* app = flipboard_alloc(
        FLIPBOARD_APP_NAME,
        &I_qr_github,
        ABOUT_TEXT,
        fields,
        flipboard_defaults,
        single_mode_button,
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
