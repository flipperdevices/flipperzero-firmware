#include "hid_cc.h"
#include "../hid.h"
#include <gui/elements.h>

#include <hid_icons.h>

#define TAG "HidCC"

struct HidCC {
    View* view;
    Hid* hid;
    FuriTimer* timer;
};

typedef struct {
    bool up_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
    bool is_cursor_set;
    float timer_duration;
    bool timer_click_enabled;
    uint8_t move_x;
    uint8_t move_y;
    uint8_t move_loop;
    uint8_t move_speed;
} HidCCModel;

static void hid_cc_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidCCModel* model = context;

    // Header
    if(model->connected) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Cookie\nClicker");
    canvas_set_font(canvas, FontSecondary);
    FuriString* buffer = furi_string_alloc();
    furi_string_printf(buffer, "%0.1f ms\r\n", (double)model->timer_duration);
    if(model->timer_click_enabled) {
        furi_string_cat(buffer, "Clicking");
    }
    elements_multiline_text_aligned(
        canvas, 17, 25, AlignLeft, AlignTop, furi_string_get_cstr(buffer));
    furi_string_free(buffer);

    // Keypad circles
    canvas_draw_icon(canvas, 76, 8, &I_Circles_47x47);

    // Up
    if(model->up_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 93, 9, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 96, 11, &I_Arr_up_7x9);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 93, 41, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 96, 44, &I_Arr_dwn_7x9);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->ok_pressed) {
        canvas_draw_icon(canvas, 91, 23, &I_Cookie_pressed_17x17);
    } else {
        canvas_draw_icon(canvas, 94, 27, &I_Cookie_def_11x9);
    }
    // Exit
    canvas_draw_icon(canvas, 0, 54, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Back to menu");
}

static void hid_cc_update_timer(HidCC* hid_cc, HidCCModel* model) {
    furi_assert(hid_cc);
    furi_assert(model);
    if(model->timer_click_enabled) {
        furi_timer_start(hid_cc->timer, model->timer_duration);
    } else {
        furi_timer_stop(hid_cc->timer);
    }
}

static void hid_cc_tick_callback(void* context) {
    furi_assert(context);
    HidCC* hid_cc = context;

    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            if(model->timer_click_enabled) {
                hid_hal_mouse_press(hid_cc->hid, HID_MOUSE_BTN_LEFT);
                furi_delay_ms(10);
                hid_hal_mouse_release(hid_cc->hid, HID_MOUSE_BTN_LEFT);
            }
        },
        true);
}

static void hid_cc_reset_cursor(HidCC* hid_cc) {
    FURI_LOG_I("HID", "Reset cursor!");
    // Move cursor from the corner
    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            // Set cursor to the phone's left up corner
            // Delays to guarantee one packet per connection interval
            for(size_t i = 0; i < 20; i++) {
                hid_hal_mouse_move(hid_cc->hid, -127, -127);
                furi_delay_ms(model->move_speed);
            }

            FURI_LOG_I("HID", "%d %d", model->move_x, model->move_y);
            for(size_t i = 0; i < model->move_loop; i++) {
                hid_hal_mouse_move(hid_cc->hid, model->move_x, model->move_y);
                furi_delay_ms(model->move_speed);
            }
        },
        true);
}

static void hid_cc_process_press(HidCC* hid_cc, HidCCModel* model, InputEvent* event) {
    UNUSED(hid_cc);
    if(event->key == InputKeyUp) {
        model->up_pressed = true;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = true;
    } else if(event->key == InputKeyOk) {
        model->ok_pressed = true;
    }
}

static void hid_cc_process_release(HidCC* hid_cc, HidCCModel* model, InputEvent* event) {
    UNUSED(hid_cc);
    if(event->key == InputKeyUp) {
        model->up_pressed = false;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = false;
    } else if(event->key == InputKeyOk) {
        model->ok_pressed = false;
    }
}

static bool hid_cc_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidCC* hid_cc = context;
    bool consumed = false;

    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            if(event->type == InputTypePress) {
                hid_cc_process_press(hid_cc, model, event);
                if(model->connected && !model->is_cursor_set) {
                    hid_cc_reset_cursor(hid_cc);
                    model->is_cursor_set = true;
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                hid_cc_process_release(hid_cc, model, event);
                consumed = true;
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyOk) {
                    model->timer_click_enabled = !model->timer_click_enabled;
                    hid_cc_update_timer(hid_cc, model);
                    consumed = true;
                } else if(event->key == InputKeyUp) {
                    // Reduce duration to 95% of value + update_timer.
                    if(model->timer_duration > 0) {
                        model->timer_duration *= 0.95f;
                        hid_cc_update_timer(hid_cc, model);
                    }
                    consumed = true;
                } else if(event->key == InputKeyDown) {
                    // Increase duration to 105% of value + update_timer
                    model->timer_duration *= 1.05f;
                    hid_cc_update_timer(hid_cc, model);
                    consumed = true;
                } else if(event->key == InputKeyBack) {
                    hid_hal_mouse_release_all(hid_cc->hid);
                    model->is_cursor_set = false;
                    consumed = false;
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyBack) {
                    hid_hal_mouse_release_all(hid_cc->hid);
                    model->is_cursor_set = false;
                    consumed = false;
                }
            }
        },
        true);

    return consumed;
}

void hid_cc_exit_callback(void* context) {
    furi_assert(context);
    HidCC* hid_cc = context;
    with_view_model(
        hid_cc->view, HidCCModel * model, { model->timer_click_enabled = false; }, true);
}

void hid_cc_set_cursor_position(HidCC* hid_cc, uint8_t x, uint8_t y, uint8_t repeat, uint8_t speed) {
    furi_assert(hid_cc);
    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            model->move_x = x;
            model->move_y = y;
            model->move_loop = repeat;
            model->move_speed = speed;
        },
        true);
}

HidCC* hid_cc_alloc(Hid* bt_hid) {
    HidCC* hid_cc = malloc(sizeof(HidCC));
    hid_cc->hid = bt_hid;
    hid_cc->view = view_alloc();
    view_set_context(hid_cc->view, hid_cc);
    view_allocate_model(hid_cc->view, ViewModelTypeLocking, sizeof(HidCCModel));
    view_set_draw_callback(hid_cc->view, hid_cc_draw_callback);
    view_set_input_callback(hid_cc->view, hid_cc_input_callback);
    view_set_exit_callback(hid_cc->view, hid_cc_exit_callback);

    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            model->timer_duration = 22.0f;
            model->timer_click_enabled = false;
        },
        true);
    hid_cc->timer = furi_timer_alloc(hid_cc_tick_callback, FuriTimerTypePeriodic, hid_cc);
    return hid_cc;
}

void hid_cc_free(HidCC* hid_cc) {
    furi_assert(hid_cc);
    furi_timer_stop(hid_cc->timer);
    furi_timer_free(hid_cc->timer);
    view_free(hid_cc->view);
    free(hid_cc);
}

View* hid_cc_get_view(HidCC* hid_cc) {
    furi_assert(hid_cc);
    return hid_cc->view;
}

void hid_cc_set_connected_status(HidCC* hid_cc, bool connected) {
    furi_assert(hid_cc);
    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            model->connected = connected;
            model->is_cursor_set = false;
        },
        true);
}
