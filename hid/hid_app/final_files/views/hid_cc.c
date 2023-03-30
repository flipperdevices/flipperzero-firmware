#include "hid_cc.h"
#include "../hid.h"
#include <gui/elements.h>

#include "hid_icons.h"

#define TAG "HidCC"

//Add a "FuriTimer* timer" to the HidCC:
struct HidCC {
    View* view;
    Hid* hid;
    FuriTimer* timer;
};

// Add a "float timer_duration" and "bool timer_click_enabled" to HidCCModel.
typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
    bool is_cursor_set;
    HidTransport transport;
    float timer_duration; // Add this line (duration in ms)
    bool timer_click_enabled; // Add this line (are we clicking)
} HidCCModel;

static void hid_cc_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidCCModel* model = context;

    // Header
    if(model->transport == HidTransportBle) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_set_font(canvas, FontPrimary);
    // Rename title from "CC" to "Cookie\nClicker". Use \n so it wraps 2 lines.
    elements_multiline_text_aligned(
        canvas, 17, 3, AlignLeft, AlignTop, "Cookie\nClicker"); // Update this line
    canvas_set_font(canvas, FontSecondary);
    // Add code to display the click speed, right after displaying the title.
    canvas_set_font(canvas, FontSecondary);
    FuriString* buffer = furi_string_alloc(32);
    furi_string_printf(buffer, "%0.1f\r\n", (double)model->timer_duration);
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

    // Left
    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 77, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 81, 29, &I_Voldwn_6x6);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 109, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 111, 29, &I_Volup_8x6);
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
    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
}

// Add a new method to update the timer.
static void hid_cc_update_timer(HidCC* hid_cc, HidCCModel* model) {
    furi_assert(hid_cc);
    furi_assert(model);

    if(model->timer_click_enabled) {
        furi_timer_start(hid_cc->timer, model->timer_duration);
    } else {
        furi_timer_stop(hid_cc->timer);
    }
}

// Add new method on tick callback that clicks button if click_enabled...
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
    // Set cursor to the phone's left up corner
    // Delays to guarantee one packet per connection interval
    for(size_t i = 0; i < 8; i++) {
        hid_hal_mouse_move(hid_cc->hid, -127, -127);
        furi_delay_ms(50);
    }
    // Move cursor from the corner
    hid_hal_mouse_move(hid_cc->hid, 20, 120);
    furi_delay_ms(50);
}

static void hid_cc_process_press(HidCC* hid_cc, HidCCModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = true;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = true;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = true;
        hid_hal_consumer_key_press(hid_cc->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = true;
        hid_hal_consumer_key_press(hid_cc->hid, HID_CONSUMER_VOLUME_INCREMENT);
    } else if(event->key == InputKeyOk) {
        model->ok_pressed = true;
    }
}

static void hid_cc_process_release(HidCC* hid_cc, HidCCModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = false;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = false;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = false;
        hid_hal_consumer_key_release(hid_cc->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = false;
        hid_hal_consumer_key_release(hid_cc->hid, HID_CONSUMER_VOLUME_INCREMENT);
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
                    // Toggle timer_click_enabled + update_timer
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
                    hid_hal_consumer_key_release_all(hid_cc->hid);
                    consumed = true;
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyBack) {
                    hid_hal_consumer_key_release_all(hid_cc->hid);
                    model->is_cursor_set = false;
                    consumed = false;
                }
            }
        },
        true);

    return consumed;
}

HidCC* hid_cc_alloc(Hid* bt_hid) {
    HidCC* hid_cc = malloc(sizeof(HidCC));
    hid_cc->hid = bt_hid;
    hid_cc->view = view_alloc();
    view_set_context(hid_cc->view, hid_cc);
    view_allocate_model(hid_cc->view, ViewModelTypeLocking, sizeof(HidCCModel));
    view_set_draw_callback(hid_cc->view, hid_cc_draw_callback);
    view_set_input_callback(hid_cc->view, hid_cc_input_callback);

    with_view_model(
        hid_cc->view,
        HidCCModel * model,
        {
            model->transport = bt_hid->transport;
            // Set default values for timer
            model->timer_duration = 22.0f;
            model->timer_click_enabled = false;
        },
        true);
    // Allocate timer
    hid_cc->timer = furi_timer_alloc(hid_cc_tick_callback, FuriTimerTypePeriodic, hid_cc);

    return hid_cc;
}

void hid_cc_free(HidCC* hid_cc) {
    furi_assert(hid_cc);
    // Free timer object.
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
