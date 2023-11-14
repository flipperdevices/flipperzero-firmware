#include "hid_ptt.h"
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include "../hid.h"
#include "../views.h"

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
    bool muted;
    bool ptt_pressed;
    bool connected;
    HidTransport transport;
} HidPttModel;

static void hid_ptt_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidPttModel* model = context;

    // Header
    canvas_set_font(canvas, FontPrimary);
    if(model->transport == HidTransportBle) {
        if(model->connected) {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
        } else {
            canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
        }
    }

    canvas_draw_icon(canvas, 5, 121, &I_ButtonLeft_4x7);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 2, 103, AlignLeft, AlignTop, "google meet");
    elements_multiline_text_aligned(canvas, 2, 112, AlignLeft, AlignTop, "mac");
    elements_multiline_text_aligned(canvas, 15, 121, AlignLeft, AlignTop, "Hold to exit");

    const uint8_t x_2 = 27;
    const uint8_t x_1 = 8;
    const uint8_t x_3 = 46;

    const uint8_t y_1 = 19;
    const uint8_t y_2 = 38;
    const uint8_t y_3 = 57;

    // Up
    canvas_draw_icon(canvas, x_2, y_1, &I_Button_18x18);
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_1 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_2 + 5, y_1 + 5, &I_Volup_8x6);
    canvas_set_color(canvas, ColorBlack);

    // Down
    canvas_draw_icon(canvas, x_2, y_3, &I_Button_18x18);
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_3 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_2 + 6, y_3 + 5, &I_Voldwn_6x6);
    canvas_set_color(canvas, ColorBlack);

    // Left
    canvas_draw_icon(canvas, x_1, y_2, &I_Button_18x18);
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, x_1 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_1 + 7, y_2 + 5, &I_ButtonLeft_4x7);
    canvas_set_color(canvas, ColorBlack);

    // Right / Camera
    canvas_draw_icon(canvas, x_3, y_2, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, x_3 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_3 + 11, y_2 + 5, &I_ButtonLeft_4x7);
    canvas_draw_box(canvas, x_3 + 4, y_2 + 5, 7, 7);
    canvas_set_color(canvas, ColorBlack);

    // Ok / Mic
    canvas_draw_icon(canvas, x_2, y_2, &I_Button_18x18);
    if(model->muted && !model->ptt_pressed) {
        canvas_draw_line(canvas, x_2 + 3, y_2 + 2, x_2 + 3 + 13, y_2 + 2 + 13);
        canvas_draw_line(canvas, x_2 + 4, y_2 + 2, x_2 + 4 + 13, y_2 + 2 + 13);
        canvas_draw_line(canvas, x_2 + 3, y_2 + 2 + 13, x_2 + 3 + 13, y_2 + 2);
        canvas_draw_line(canvas, x_2 + 4, y_2 + 2 + 13, x_2 + 4 + 13, y_2 + 2);
    }
    canvas_draw_icon(canvas, x_2 + 5, y_2 + 4, &I_Mic_btn_9x9);
    canvas_set_color(canvas, ColorBlack);

    // Back / PTT
    canvas_draw_icon(canvas, x_2, 0, &I_Space_60x18);
    if(model->ptt_pressed) {
        elements_slightly_rounded_box(canvas, x_2+3, 0+2, 30, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_2+3, 0+4, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, x_2+15, 0+12, AlignLeft, AlignBottom, "PTT");
}

static void hid_ptt_process(HidPtt* hid_ptt, InputEvent* event) {
    with_view_model(
        hid_ptt->view,
        HidPttModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                } else if(event->key == InputKeyBack) {
                    model->ptt_pressed = true;
                    if (model->muted) {
                        hid_hal_keyboard_press(hid_ptt->hid, HID_KEYBOARD_SPACEBAR);
                    }
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                    hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                    hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;

                } else if(event->key == InputKeyBack) {
                    model->ptt_pressed = false;
                    if (model->muted) {
                        hid_hal_keyboard_release(hid_ptt->hid, HID_KEYBOARD_SPACEBAR); // release PTT
                    } else {
                        // mute
                        hid_hal_keyboard_press(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D);
                        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D );
                        model->muted = true;
                    }
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyOk && !model->ptt_pressed ) { // no changes if PTT is pressed
                    model->muted = !model->muted;
                    //  + d
                    hid_hal_keyboard_press(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D);
                    hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D);
                } else if(event->key == InputKeyRight) {
                    //  + d
                    hid_hal_keyboard_press(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_E);
                    hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_E);
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                    hid_hal_keyboard_release_all(hid_ptt->hid);
                    view_dispatcher_switch_to_view(hid_ptt->hid->view_dispatcher, HidViewSubmenu);
                    // sequence_double_vibro to notify that we quit PTT
                    notification_message(hid_ptt->hid->notifications, &sequence_double_vibro);
                } else if(event->key == InputKeyOk && !model->ptt_pressed ) { // no changes if PTT is pressed
                    // Change local mic status
                    model->muted = !model->muted;
                }
            }
            //LED
            if (model->muted && !model->ptt_pressed) {
                notification_message(hid_ptt->hid->notifications, &sequence_reset_red);
            } else {
                notification_message(hid_ptt->hid->notifications, &sequence_set_red_255);
            }
        },
        true);
}

static bool hid_ptt_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidPtt* hid_ptt = context;
    bool consumed = true;
    hid_ptt_process(hid_ptt, event);
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
    view_set_orientation(hid_ptt->view, ViewOrientationVerticalFlip);

    with_view_model(
        hid_ptt->view, HidPttModel * model, {
            model->transport = hid->transport;
            model->muted = false; // assume we're not muted
        }, true);
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
