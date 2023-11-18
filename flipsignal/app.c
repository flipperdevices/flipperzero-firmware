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

static bool x(void* context, uint32_t event) {
    if(event == 1) {
        FuriString* sub_file_contents = furi_string_alloc();
        furi_string_printf(
            sub_file_contents,
            "Filetype: Flipper SubGhz Key File\r\n"
            "Version: 1\r\n"
            "Frequency: 433920000\r\n"
            "Preset: FuriHalSubGhzPresetOok650Async\r\n"
            "Protocol: Princeton\r\n"
            "Bit: 24\r\n"
            "Key: 00 00 00 00 00 52 81 1C\r\n"
            "TE: 157\r\n");
        send_signal("Princeton", 433920000, sub_file_contents, false);
        furi_string_free(sub_file_contents);
    } else if(event == 2) {
        FuriString* sub_file_contents = furi_string_alloc();
        furi_string_printf(
            sub_file_contents,
            "Filetype: Flipper SubGhz Key File\r\n"
            "Version: 1\r\n"
            "Frequency: 433920000\r\n"
            "Preset: FuriHalSubGhzPresetOok650Async\r\n"
            "Protocol: Princeton\r\n"
            "Bit: 24\r\n"
            "Key: 00 00 00 00 00 52 81 14\r\n"
            "TE: 157\r\n");
        send_signal("Princeton", 433920000, sub_file_contents, false);
        furi_string_free(sub_file_contents);
    } else if(event == 4) {
        FuriString* sub_file_contents = furi_string_alloc();
        furi_string_printf(
            sub_file_contents,
            "Filetype: Flipper SubGhz Key File\r\n"
            "Version: 1\r\n"
            "Frequency: 433920000\r\n"
            "Preset: FuriHalSubGhzPresetOok650Async\r\n"
            "Protocol: Security+ 1.0\r\n"
            "Bit: 42\r\n"
            "Key: 63 A4 A7 6D E6 00 00 00\r\n");
        send_signal("Security+ 1.0", 433920000, sub_file_contents, false);
        furi_string_free(sub_file_contents);
    }

    return true;
}

/**
 * @brief This method transmits a signal associated with the button.
 * @param model The FlipboardModel* context.
 * @param bm The ButtonModel* context.
 */
static ViewDispatcher* view_dispatcher = NULL;
static void flipboard_model_send_signal(FlipboardModel* model, ButtonModel* bm) {
    UNUSED(model);
    if(bm == NULL) {
        // Key up event.
        return;
    }

    uint8_t btn = button_model_get_button_id(bm);
    FURI_LOG_D(TAG, "TODO: Send signal %d.SUB", btn);

    view_dispatcher_send_custom_event(view_dispatcher, btn);
}

/**
 * @brief This method handles FlipBoard button input when in the FlipboardKeyboard mode.
 * @param context The Flipboard* context.
 * @param old_key The previous key state.
 * @param new_key The new key state.
 * @return true if the key event was handled, false otherwise.
 */
static bool flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

    FURI_LOG_D(
        TAG, "SW EVENT: old=%d new=%d reduced=%d", old_button, new_button, reduced_new_button);

    ButtonModel* bm = flipboard_model_get_button_model(model, reduced_new_button);
    flipboard_model_set_colors(model, bm, new_button);
    flipboard_model_play_tone(model, bm);
    flipboard_model_send_signal(model, bm);

    return true;
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
 * @brief This method is invoked when the FlipboardKeyboard app is launched.
 * @param p Unused.
 * @return 0.
 */
int32_t flipboard_signal_app(void* p) {
    UNUSED(p);

    ButtonModelFields fields = ButtonModelFieldColorDown | ButtonModelFieldColorUp |
                               ButtonModelFieldFrequency;
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
    view_dispatcher_set_custom_event_callback(flipboard_get_view_dispatcher(app), x);
    view_dispatcher_run(flipboard_get_view_dispatcher(app));
    flipboard_free(app);

    return 0;
}
