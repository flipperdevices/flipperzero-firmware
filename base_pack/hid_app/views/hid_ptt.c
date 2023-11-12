#include "hid_ptt.h"
#include <furi.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidPtt"

struct HidPtt {
    View* view;
    Hid* hid;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
    bool back_pressed;
    HidTransport transport;
} HidPttModel;

static void hid_ptt_draw_arrow(Canvas* canvas, uint8_t x, uint8_t y, CanvasDirection dir) {
    canvas_draw_triangle(canvas, x, y, 5, 3, dir);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_dot(canvas, x, y - 1);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_dot(canvas, x, y + 1);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_dot(canvas, x - 1, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_dot(canvas, x + 1, y);
    }
}

static void hid_ptt_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidPttModel* model = context;

    // Header
    if(model->transport == HidTransportBle) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "PTT");
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 0, 12, AlignLeft, AlignTop, "Google Meet");
    elements_multiline_text_aligned(canvas, 0, 22, AlignLeft, AlignTop, "Mac");

    // Keypad circles
    canvas_draw_icon(canvas, 58, 3, &I_OutCircles_70x51);

    // Up
    if(model->up_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 68, 6, &I_S_UP_31x15);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 79, 9, &I_Volup_8x6);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 68, 36, &I_S_DOWN_31x15);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 80, 41, &I_Voldwn_6x6);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 61, 13, &I_S_LEFT_15x31);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_ptt_draw_arrow(canvas, 65, 28, CanvasDirectionRightToLeft);
    hid_ptt_draw_arrow(canvas, 70, 28, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 91, 13, &I_S_RIGHT_15x31);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_ptt_draw_arrow(canvas, 96, 28, CanvasDirectionLeftToRight);
    hid_ptt_draw_arrow(canvas, 101, 28, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->ok_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 74, 19, &I_Pressed_Button_19x19);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_ptt_draw_arrow(canvas, 80, 28, CanvasDirectionLeftToRight);
    canvas_draw_line(canvas, 84, 26, 84, 30);
    canvas_draw_line(canvas, 86, 26, 86, 30);
    canvas_set_color(canvas, ColorBlack);

    // Exit
    if(model->back_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 107, 33, &I_Pressed_Button_19x19);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 111, 38, &I_Pin_back_arrow_10x10);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_icon(canvas, 0, 54, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
}

static void hid_ptt_process_press(HidPtt* hid_ptt, InputEvent* event) {
    with_view_model(
        hid_ptt->view,
        HidPttModel * model,
        {
            if(event->key == InputKeyUp) {
                model->up_pressed = true;
                hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
            } else if(event->key == InputKeyDown) {
                model->down_pressed = true;
                hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = true;
                hid_hal_keyboard_press(hid_ptt->hid, HID_KEYBOARD_LEFT_ARROW);
            } else if(event->key == InputKeyRight) {
                model->right_pressed = true;
                hid_hal_keyboard_press(hid_ptt->hid, HID_KEYBOARD_RIGHT_ARROW);
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = true;
                hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_PLAY_PAUSE);
            } else if(event->key == InputKeyBack) {
                model->back_pressed = true;
            }
        },
        true);
}

static void hid_ptt_process_release(HidPtt* hid_ptt, InputEvent* event) {
    with_view_model(
        hid_ptt->view,
        HidPttModel * model,
        {
            if(event->key == InputKeyUp) {
                model->up_pressed = false;
                hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
            } else if(event->key == InputKeyDown) {
                model->down_pressed = false;
                hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = false;
                hid_hal_keyboard_release(hid_ptt->hid, HID_KEYBOARD_LEFT_ARROW);
            } else if(event->key == InputKeyRight) {
                model->right_pressed = false;
                hid_hal_keyboard_release(hid_ptt->hid, HID_KEYBOARD_RIGHT_ARROW);
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = false;
                hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_PLAY_PAUSE);
            } else if(event->key == InputKeyBack) {
                model->back_pressed = false;
            }
        },
        true);
}

static bool hid_ptt_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidPtt* hid_ptt = context;
    bool consumed = false;

    if(event->type == InputTypePress) {
        hid_ptt_process_press(hid_ptt, event);
        consumed = true;
    } else if(event->type == InputTypeRelease) {
        hid_ptt_process_release(hid_ptt, event);
        consumed = true;
    }
    return consumed;
}

HidPtt* hid_ptt_alloc(Hid* hid) {
    HidPtt* hid_ptt = malloc(sizeof(HidPtt));
    hid_ptt->view = view_alloc();
    hid_ptt->hid = hid;
    view_set_context(hid_ptt->view, hid_ptt);
    view_allocate_model(hid_ptt->view, ViewModelTypeLocking, sizeof(HidPttModel));
    view_set_draw_callback(hid_ptt->view, hid_ptt_draw_callback);
    view_set_input_callback(hid_ptt->view, hid_ptt_input_callback);

    with_view_model(
        hid_ptt->view, HidPttModel * model, { model->transport = hid->transport; }, true);

    return hid_ptt;
}

void hid_ptt_free(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    view_free(hid_ptt->view);
    free(hid_ptt);
}

View* hid_ptt_get_view(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    return hid_ptt->view;
}

void hid_ptt_set_connected_status(HidPtt* hid_ptt, bool connected) {
    furi_assert(hid_ptt);
    with_view_model(
        hid_ptt->view, HidPttModel * model, { model->connected = connected; }, true);
}
