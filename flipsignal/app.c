#include "app.h"
#include "app_config.h"

#include <devices/devices.h>

/**
 * @brief This method handles Flipper D-Pad input when in the FlipboardSignal mode.
 * @param event The InputEvent* to handle.
 * @param context The Flipboard* context.
 * @return true if the key event was handled, false otherwise.
 */
static bool flipboard_view_flip_signal_input(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

/**
 * @brief This method handles drawing when in the FlipboardSignal mode.
 * @param canvas The canvas to draw on.
 * @param model The FlipboardModelRef* context.
 */
static void flipboard_view_flip_signal_draw(Canvas* canvas, void* model) {
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
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 84, 60, furi_string_get_cstr(action_text));

    furi_string_free(action_text);
}

/**
 * @brief This method sends a subghz signal.
 * @details This method can send a subghz signal from a .SUB file (both RAW 
 * and protocols are supported).
 * @param model The FlipboardModel* context.
 * @param filename The filename of the signal to send.
 */
static void send_subghz(FlipboardModel* model, char* filename) {
    // TODO: Add cancel API for subghz RAW by using ButtonMonitor.
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    Resources* resources = flipboard_model_get_resources(model);
    SubGhzSignal* signal = subghz_signal_load_file(filename);
    flipboard_status_led(leds, true);
    subghz_signal_send(signal, false, resources);
    flipboard_status_led(leds, false);
    subghz_signal_free(signal);
}

/**
 * @brief This method sends an infrared signal.
 * @details This method can send an infrared signal from a .IR file.
 * @param model The FlipboardModel* context.
 * @param filename The filename of the signal to send.
 * @param action_name The action to send (matches the name parameter in the file).
 * @return true if the signal was sent, false otherwise.
 */
static bool send_ir(FlipboardModel* model, char* filename, char* action_name) {
    bool sent_signal = false;
    ButtonMonitor* button_monitor = flipboard_model_get_button_monitor(model);
    Resources* resources = flipboard_model_get_resources(model);
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    InfraredSignal* ir_signal = infrared_signal_load_file(filename, action_name, resources);
    do {
        flipboard_status_led(leds, true);
        sent_signal |= infrared_signal_send(ir_signal);
        flipboard_status_led(leds, false);
    } while(infrared_signal_load_next(ir_signal) &&
            button_monitor_get_last_status(button_monitor));
    infrared_signal_free(ir_signal);
    return sent_signal;
}

/**
 * @brief This method transmits a signal associated with the button.
 * @param model The FlipboardModel* context.
 * @param action_model The ActionModel* context.
 */
static ViewDispatcher* view_dispatcher = NULL;
static void flipboard_model_send_signal(FlipboardModel* model, ActionModel* action_model) {
    UNUSED(model);
    if(action_model == NULL) {
        // Key up event.
        return;
    }

    uint8_t action = action_model_get_action_id(action_model);

    view_dispatcher_send_custom_event(view_dispatcher, CustomEventFlipboardButtonPress + action);
}

/**
 * @brief This method handles FlipBoard button input when in the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 * @param old_key The previous key state.
 * @param new_key The new key state.
 */
static void flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

    FURI_LOG_D(
        TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);

    flipboard_model_update_gui(model);

    ActionModel* action_model = flipboard_model_get_action_model(model, reduced_new_button);
    flipboard_model_set_colors(model, action_model, new_button);
    flipboard_model_play_tone(model, action_model);
    flipboard_model_send_signal(model, action_model);
}

/**
 * @brief This method is invoked when entering the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
static void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_button_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
}

/**
 * @brief This method is invoked when exiting the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 */
static void flipboard_exit_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    flipboard_model_set_colors(fm, NULL, 0x0);
    flipboard_model_set_button_monitor(fm, NULL, NULL);
}

/**
 * @brief This method configures the View* used for FlipboardSignal mode.
 * @param context The Flipboard* context.
 * @return View* for FlipboardSignal mode.
 */
static View* get_primary_view(void* context) {
    FlipboardModel* model = flipboard_get_model((Flipboard*)context);
    View* view_primary = view_alloc();
    view_set_draw_callback(view_primary, flipboard_view_flip_signal_draw);
    view_set_input_callback(view_primary, flipboard_view_flip_signal_input);
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
        flipboard_leds_reset(leds);
        flipboard_leds_set(leds, LedId1, LedColorRed);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId2, LedColorCyan);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId3, LedColorMagenta);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId4, LedColorViolet);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId1, LedColorBlack);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId2, LedColorBlack);
        flipboard_leds_update(leds);
        furi_delay_ms(100);

        flipboard_leds_set(leds, LedId3, LedColorBlack);
        flipboard_leds_update(leds);
        furi_delay_ms(100);
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
    } else if(event == CustomEventFlipboardButtonPress + 1) {
        send_subghz(model, "/ext/subghz/flip1.sub");
        if(!send_ir(model, "/ext/infrared/flipboard.ir", "Flip1")) {
            send_ir(model, "/ext/infrared/assets/tv.ir", "Power");
        }
    } else if(event == CustomEventFlipboardButtonPress + 2) {
        send_subghz(model, "/ext/subghz/flip2.sub");
        if(!send_ir(model, "/ext/infrared/flipboard.ir", "Flip2")) {
            send_ir(model, "/ext/infrared/assets/tv.ir", "Mute");
        }
    } else if(event == CustomEventFlipboardButtonPress + 4) {
        send_subghz(model, "/ext/subghz/flip4.sub");
        if(!send_ir(model, "/ext/infrared/flipboard.ir", "Flip4")) {
            send_ir(model, "/ext/infrared/assets/tv.ir", "Ch_prev");
        }
    } else if(event == CustomEventFlipboardButtonPress + 8) {
        send_subghz(model, "/ext/subghz/flip8.sub");
        if(!send_ir(model, "/ext/infrared/flipboard.ir", "Flip8")) {
            send_ir(model, "/ext/infrared/assets/tv.ir", "Ch_next");
        }
    }

    return true;
}

/**
 * @brief This method is invoked when the FlipboardKeyboard app is launched.
 * @param p Unused.
 * @return 0.
 */
int32_t flipboard_signal_app(void* p) {
    UNUSED(p);

    ActionModelFields fields = ActionModelFieldColorDown | ActionModelFieldColorUp |
                               ActionModelFieldFrequency;
    bool single_mode_button = true;
    bool attach_keyboard = false;

    Flipboard* app = flipboard_alloc(
        FLIPBOARD_APP_NAME,
        FLIPBOARD_PRIMARY_ITEM_NAME,
        ABOUT_TEXT,
        fields,
        single_mode_button,
        attach_keyboard,
        NULL,
        NULL,
        0,
        get_primary_view);
    view_dispatcher = flipboard_get_view_dispatcher(app);
    view_dispatcher_set_event_callback_context(flipboard_get_view_dispatcher(app), app);
    view_dispatcher_set_custom_event_callback(
        flipboard_get_view_dispatcher(app), custom_event_handler);
    view_dispatcher_run(flipboard_get_view_dispatcher(app));
    flipboard_free(app);

    return 0;
}
