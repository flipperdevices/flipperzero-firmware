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
    bool mic_pressed;
    bool connected;
    bool is_mac_os;
    uint32_t appIndex;
    HidTransport transport;
} HidPttModel;

enum HidPttAppIndex {
    HidPttAppIndexGoogleMeet,
    HidPttAppIndexZoom,
    HidPttAppIndexFaceTime,
    HidPttAppIndexSkype,
    HidPttAppIndexSize,
};

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

    // App selection
    const uint8_t y_app = 78;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_icon(canvas, 0, y_app, &I_ButtonLeft_4x7);
    if(model->appIndex == HidPttAppIndexGoogleMeet) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Google Meet");
    } else if(model->appIndex == HidPttAppIndexZoom) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Zoom");
    } else if(model->appIndex == HidPttAppIndexFaceTime) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "FaceTime");
    } else if(model->appIndex == HidPttAppIndexSkype) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Skype");
    }
    canvas_draw_icon(canvas, 60, y_app, &I_ButtonRight_4x7);

    // OS selection
    const uint8_t y_os = 88;
    const uint8_t x_os = 7;
    // elements_slightly_rounded_box(canvas, model->is_mac_os ? 0 : 26, y_os, model->is_mac_os ? 21 : 26, 11);
    elements_slightly_rounded_box(canvas, model->is_mac_os ? x_os : x_os + 26, y_os, model->is_mac_os ? 21 : 26, 11);
    canvas_set_color(canvas, model->is_mac_os ? ColorWhite : ColorBlack);
    elements_multiline_text_aligned(canvas, x_os + 2, y_os + 1, AlignLeft, AlignTop, "Mac");
    canvas_set_color(canvas, ColorBlack);
    if (model->appIndex != HidPttAppIndexFaceTime) {
        elements_multiline_text_aligned(canvas, x_os + 23, y_os + 2, AlignLeft, AlignTop, "|");
        canvas_set_color(canvas, model->is_mac_os ? ColorBlack : ColorWhite);
        elements_multiline_text_aligned(canvas, x_os + 28, y_os + 2, AlignLeft, AlignTop, "Linux");
        canvas_set_color(canvas, ColorBlack);
    }

    // Mic label
    const uint8_t y_mic = 102;
    canvas_draw_icon(canvas, 19, y_mic - 1, &I_Pin_back_arrow_rotated_8x10);
    elements_multiline_text_aligned(canvas, 0, y_mic, AlignLeft, AlignTop, "Hold      to sync");
    elements_multiline_text_aligned(canvas, 20, y_mic+10, AlignLeft, AlignTop, "mic status");

    // Exit label
    canvas_draw_icon(canvas, 20, 121, &I_ButtonLeft_4x7);
    elements_multiline_text_aligned(canvas, 0, 121, AlignLeft, AlignTop, "Hold    to exit");

    const uint8_t x_1 = 0;
    const uint8_t x_2 = x_1 + 19 + 4;
    const uint8_t x_3 = x_1 + 19 * 2 + 8;

    const uint8_t y_1 = 19;
    const uint8_t y_2 = y_1 + 19;
    const uint8_t y_3 = y_2 + 19;

    // Up
    canvas_draw_icon(canvas, x_2, y_1, &I_Button_18x18);
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_1 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if(model->ptt_pressed) {
        if (model->appIndex != HidPttAppIndexFaceTime) {
            elements_multiline_text_aligned(canvas, x_2 + 4, y_1 + 5, AlignLeft, AlignTop, "OS");
        }
    } else {
        canvas_draw_icon(canvas, x_2 + 5, y_1 + 5, &I_Volup_8x6);
    }
    canvas_set_color(canvas, ColorBlack);

    // Down
    canvas_draw_icon(canvas, x_2, y_3, &I_Button_18x18);
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, x_2 + 3, y_3 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if(!model->ptt_pressed) {
        canvas_draw_icon(canvas, x_2 + 6, y_3 + 5, &I_Voldwn_6x6);
    }
    canvas_set_color(canvas, ColorBlack);

    // Left
    canvas_draw_icon(canvas, x_1, y_2, &I_Button_18x18);
    if(model->left_pressed) {                                             
        elements_slightly_rounded_box(canvas, x_1 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if (model->ptt_pressed) {
        canvas_draw_icon(canvas, x_1 + 7, y_2 + 5, &I_ButtonLeft_4x7);
    } else {  
        canvas_draw_icon(canvas, x_1 + 4, y_2 + 5, &I_Pin_back_arrow_10x8);
    }
    canvas_set_color(canvas, ColorBlack);

    // Right / Camera
    canvas_draw_icon(canvas, x_3, y_2, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, x_3 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if(!model->ptt_pressed) {
        if (model->appIndex != HidPttAppIndexFaceTime) {
            canvas_draw_icon(canvas, x_3 + 11, y_2 + 5, &I_ButtonLeft_4x7);
            canvas_draw_box(canvas, x_3 + 4, y_2 + 5, 7, 7);
        }
    } else {
        canvas_draw_icon(canvas, x_3 + 8, y_2 + 5, &I_ButtonRight_4x7);
    }
    canvas_set_color(canvas, ColorBlack);

    // Back / Mic
    const uint8_t x_mic = x_3;
    canvas_draw_icon(canvas, x_mic, 0, &I_Button_18x18);
    if(model->mic_pressed) {
        elements_slightly_rounded_box(canvas, x_mic + 3, 0 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x_mic + 5, 0 + 4, &I_Mic_btn_8x10);
    if(model->muted && !model->ptt_pressed) {
        canvas_draw_line(canvas, x_mic + 3, 2     , x_mic + 3 + 13, 2 + 13);
        canvas_draw_line(canvas, x_mic + 2, 2     , x_mic + 2 + 13, 2 + 13);
        canvas_draw_line(canvas, x_mic + 3, 2 + 13, x_mic + 3 + 13, 2);
        canvas_draw_line(canvas, x_mic + 2, 2 + 13, x_mic + 2 + 13, 2);
    }
    canvas_set_color(canvas, ColorBlack);

    // Ok / PTT
    const uint8_t x_ptt_margin = 4;
    const uint8_t x_ptt_width = 17;
    const uint8_t x_ptt = x_1 + 19;
    canvas_draw_icon(canvas, x_ptt                                 , y_2     , &I_BtnFrameLeft_3x18);
    canvas_draw_icon(canvas, x_ptt + x_ptt_width + 3 + x_ptt_margin, y_2     , &I_BtnFrameRight_2x18);
    canvas_draw_line(canvas, x_ptt + 3                             , y_2     , x_ptt + x_ptt_width + 2 + x_ptt_margin, y_2);
    canvas_draw_line(canvas, x_ptt + 3                             , y_2 + 16, x_ptt + x_ptt_width + 2 + x_ptt_margin, y_2 + 16);
    canvas_draw_line(canvas, x_ptt + 3                             , y_2 + 17, x_ptt + x_ptt_width + 2 + x_ptt_margin, y_2 + 17);
    if(model->ptt_pressed) {
        elements_slightly_rounded_box(canvas, x_ptt + 3, y_2 + 2, x_ptt_width + x_ptt_margin, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, x_ptt + 2 + x_ptt_margin / 2, y_2 + 13, AlignLeft, AlignBottom, "PTT");
    canvas_set_font(canvas, FontSecondary);
}

static void hid_ptt_trigger_mute(HidPtt* hid_ptt, HidPttModel * model) {
    if(model->appIndex == HidPttAppIndexGoogleMeet && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_D );
    } else if(model->appIndex == HidPttAppIndexGoogleMeet && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_D);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_D );
    } else if(model->appIndex == HidPttAppIndexZoom && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_A);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_A );
    } else if(model->appIndex == HidPttAppIndexFaceTime) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M );
    }
}

static void hid_ptt_trigger_camera(HidPtt* hid_ptt, HidPttModel * model) {
    if(model->appIndex == HidPttAppIndexGoogleMeet && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_E);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | HID_KEYBOARD_E );
    } else if(model->appIndex == HidPttAppIndexGoogleMeet && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_E);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_E );
    } else if(model->appIndex == HidPttAppIndexZoom && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_V);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_V );
    } else if(model->appIndex == HidPttAppIndexZoom && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_ALT | HID_KEYBOARD_V);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_ALT | HID_KEYBOARD_V );
    } else if(model->appIndex == HidPttAppIndexSkype && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_K);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_K );
    } else if(model->appIndex == HidPttAppIndexSkype && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_K);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_K );
    }
}

static void hid_ptt_start_ptt(HidPtt* hid_ptt, HidPttModel * model) {
    if(model->appIndex == HidPttAppIndexGoogleMeet) {
        hid_hal_keyboard_press(hid_ptt->hid, HID_KEYBOARD_SPACEBAR);
    } else if(model->appIndex == HidPttAppIndexZoom) {
        hid_hal_keyboard_press(hid_ptt->hid, HID_KEYBOARD_SPACEBAR);
    } else if(model->appIndex == HidPttAppIndexFaceTime) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M );   
    }
}

static void hid_ptt_stop_ptt(HidPtt* hid_ptt, HidPttModel * model) {
    if(model->appIndex == HidPttAppIndexGoogleMeet) {
        hid_hal_keyboard_release(hid_ptt->hid, HID_KEYBOARD_SPACEBAR);
    } else if(model->appIndex == HidPttAppIndexZoom) {
        hid_hal_keyboard_release(hid_ptt->hid, HID_KEYBOARD_SPACEBAR);
    } else if(model->appIndex == HidPttAppIndexFaceTime) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| KEY_MOD_LEFT_SHIFT | HID_KEYBOARD_M );
    } else if(model->appIndex == HidPttAppIndexSkype && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | HID_KEYBOARD_M );
    }
}

// Supports only ±1
static void hid_ptt_shift_app(HidPttModel * model, int shift) {
    int i = (short) model->appIndex;
    if (i + shift >= HidPttAppIndexSize) {
        model->appIndex = 0;
    } else if(i + shift <= 0) {
        model->appIndex = HidPttAppIndexSize - 1;
    } else {
        model->appIndex += shift;
    }
    // Avoid showing facetime if not macos
    if (model->appIndex == HidPttAppIndexFaceTime && !model->is_mac_os) {
        hid_ptt_shift_app(model, shift);
    }
}

static void hid_ptt_process(HidPtt* hid_ptt, InputEvent* event) {
    with_view_model(
        hid_ptt->view,
        HidPttModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    if (!model->ptt_pressed){
                        hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                    } else {
                        if (model->appIndex != HidPttAppIndexFaceTime) {
                            model->is_mac_os = !model->is_mac_os;
                            notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                        }
                    }
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    if (!model->ptt_pressed){
                        hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                    } else {
                        hid_ptt_shift_app(model, - 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    if (model->ptt_pressed){
                        hid_ptt_shift_app(model, 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    if (model->ptt_pressed){
                        hid_ptt_shift_app(model, - 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyOk) {
                    model->ptt_pressed = true;
                    if (model->muted) {
                        hid_ptt_start_ptt(hid_ptt, model);
                    }
                } else if(event->key == InputKeyBack) {
                    model->mic_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                    if (!model->ptt_pressed){
                        hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                    }
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                    if (!model->ptt_pressed){
                        hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                    }
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;

                } else if(event->key == InputKeyOk) {
                    model->ptt_pressed = false;
                    if (model->muted) {
                        hid_ptt_stop_ptt(hid_ptt, model);
                    } else {
                        hid_ptt_trigger_mute(hid_ptt, model);
                        model->muted = true;
                    }
                } else if(event->key == InputKeyBack) {
                    model->mic_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack && !model->ptt_pressed ) { // no changes if PTT is pressed
                    model->muted = !model->muted;
                    hid_ptt_trigger_mute(hid_ptt, model);
                } else if(event->key == InputKeyRight) {
                    if (!model->ptt_pressed){
                        hid_ptt_trigger_camera(hid_ptt, model);
                    }
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                    if (!model->ptt_pressed){
                        hid_hal_keyboard_release_all(hid_ptt->hid);
                        view_dispatcher_switch_to_view(hid_ptt->hid->view_dispatcher, HidViewSubmenu);
                        // sequence_double_vibro to notify that we quit PTT
                        notification_message(hid_ptt->hid->notifications, &sequence_double_vibro);
                    }
                } else if(event->key == InputKeyBack && !model->ptt_pressed ) { // no changes if PTT is pressed
                    // Change local mic status
                    model->muted = !model->muted;
                    notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
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
            model->muted = true; // assume we're muted
            model->is_mac_os = true;
        }, true);
    return hid_ptt;
}

void hid_ptt_free(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    notification_message(hid_ptt->hid->notifications, &sequence_reset_red);
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
