#include "app.h"
#include "app_config.h"
#include "flipboard_blinky_icons.h"

#include <gui/modules/widget.h>
#include <furi.h>

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
    FlipboardModelRef* my_model = (FlipboardModelRef*)model;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_icon(canvas, 1, 1, &I_nametag);

    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(my_model->model);
    if(fbm->detail_counter > 0) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 0, 48, 128, 64 - 48);
        canvas_set_color(canvas, ColorBlack);
        FuriString* str = furi_string_alloc();
        furi_string_printf(
            str,
            "Speed:%lu    Effect:%d of %d",
            fbm->period_ms,
            fbm->effect_id,
            fbm->max_effect_id);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 1, 61, furi_string_get_cstr(str));
        furi_string_free(str);
        fbm->detail_counter--;
    }
}

void flipboard_reset_effect(FlipboardModel* model) {
    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(model);
    fbm->colors[0] = 0xFF0000;
    fbm->colors[1] = 0xFFFFFF;
    fbm->colors[2] = 0xFF0000;
    fbm->colors[3] = 0x0000FF;
}

void flipboard_do_effect(FlipboardModel* model) {
    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(model);
    FlipboardLeds* leds = flipboard_model_get_leds(model);

    uint32_t tmp = fbm->colors[0];
    for(int i = 0; i < 3; i++) {
        fbm->colors[i] = fbm->colors[i + 1];
    }
    fbm->colors[3] = tmp;
    flipboard_leds_set(leds, LedId1, fbm->colors[0]);
    flipboard_leds_set(leds, LedId2, fbm->colors[1]);
    flipboard_leds_set(leds, LedId3, fbm->colors[2]);
    flipboard_leds_set(leds, LedId4, fbm->colors[3]);
    flipboard_leds_update(leds);
}

/*
This method handles FlipBoard key input when in the FlipboardKeyboard mode.
*/
bool flipboard_debounced_switch(void* context, uint8_t old_key, uint8_t new_key) {
    Flipboard* app = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(app);
    uint8_t reduced_new_key = flipboard_model_reduce(model, new_key, false);
    uint32_t detail_counter_ticks = 20;

    FURI_LOG_D(TAG, "SW EVENT: old=%d new=%d reduced=%d", old_key, new_key, reduced_new_key);

    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(model);
    if((new_key == 1) || (new_key == 3)) {
        // Faster by 5ms
        uint32_t delay = (new_key == 1) ? 5 : 20;
        if(fbm->period_ms > delay) {
            fbm->period_ms -= delay;
            if(fbm->period_ms < 20) {
                fbm->period_ms = 20;
            }
            furi_timer_start(fbm->timer, furi_ms_to_ticks(fbm->period_ms));
            fbm->detail_counter = detail_counter_ticks;
        }
    } else if(new_key == 2) {
        // Slower by 20ms
        uint32_t delay = 20;
        fbm->period_ms += delay;
        furi_timer_start(fbm->timer, furi_ms_to_ticks(fbm->period_ms));
        fbm->detail_counter = detail_counter_ticks;
    } else if(new_key == 4) {
        // Previous effect
        fbm->effect_id--;
        if(fbm->effect_id < 1) {
            fbm->effect_id = fbm->max_effect_id;
        }
        fbm->detail_counter = detail_counter_ticks;
    } else if(new_key == 8) {
        // Next effect
        fbm->effect_id++;
        if(fbm->effect_id > fbm->max_effect_id) {
            fbm->effect_id = 1;
        }
        fbm->detail_counter = detail_counter_ticks;
        flipboard_reset_effect(model);
    } else if(new_key == 12) {
        // Max effect
        fbm->effect_id = fbm->max_effect_id;
        fbm->detail_counter = detail_counter_ticks;
        flipboard_reset_effect(model);
    }

    return true;
}

void flipboard_tick_callback(void* context) {
    FlipboardModel* model = (FlipboardModel*)context;
    flipboard_do_effect(model);
}

/*
This method is invoked when entering the FlipboardKeyboard mode.
*/
void flipboard_enter_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(fm);
    flipboard_model_set_key_monitor(fm, flipboard_debounced_switch, (Flipboard*)context);
    furi_timer_start(fbm->timer, furi_ms_to_ticks(fbm->period_ms));
    flipboard_model_set_gui_refresh_speed_ms(fm, 1000);
}

/*
This method is invoked when exiting the FlipboardKeyboard mode.
*/
void flipboard_exit_callback(void* context) {
    FlipboardModel* fm = flipboard_get_model((Flipboard*)context);
    FlipboardBlinkyModel* fbm = flipboard_model_get_custom_data(fm);
    furi_timer_stop(fbm->timer);
    flipboard_model_set_key_monitor(fm, NULL, NULL);
    flipboard_model_set_gui_refresh_speed_ms(fm, 0);
    flipboard_model_set_colors(fm, NULL, 0x0);
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

FlipboardBlinkyModel* flipboard_blinky_model_alloc(FlipboardModel* context) {
    FlipboardBlinkyModel* fbm = malloc(sizeof(FlipboardBlinkyModel));
    fbm->timer = furi_timer_alloc(flipboard_tick_callback, FuriTimerTypePeriodic, context);
    fbm->period_ms = 200;
    fbm->effect_id = 1;
    fbm->max_effect_id = 3;
    fbm->detail_counter = 0;
    return fbm;
}

void flipboard_blinky_model_free(FlipboardBlinkyModel* fbm) {
    if(fbm->timer) {
        furi_timer_free(fbm->timer);
    }
    free(fbm);
}

int32_t flipboard_blinky_app(void* p) {
    UNUSED(p);

    KeySettingModelFields fields = KeySettingModelFieldNone;
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

    Widget* widget = widget_alloc();
    widget_add_text_scroll_element(
        widget, 0, 0, 128, 64, "TODO: Add config screen!\n\nPress BACK for now.");
    view_set_previous_callback(widget_get_view(widget), flipboard_navigation_show_app_menu);
    flipboard_override_config_view(app, widget_get_view(widget));
    FlipboardModel* model = flipboard_get_model(app);
    FlipboardBlinkyModel* fbm = flipboard_blinky_model_alloc(model);
    flipboard_model_set_custom_data(model, fbm);
    flipboard_reset_effect(model);

    view_dispatcher_run(flipboard_get_view_dispatcher(app));

    flipboard_blinky_model_free(fbm);
    flipboard_free(app);

    return 0;
}
