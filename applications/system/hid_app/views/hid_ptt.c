#include "hid_ptt.h"
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <m-array.h>
#include "../hid.h"
#include "../views.h"

#include "hid_icons.h"

#define TAG "HidPtt"

struct HidPtt {
    View* view;
    View* menuView;
    Hid* hid;
};

// Menu

typedef struct {
    FuriString* label;
    uint32_t index;
} PttMenuItem;

static void PttMenuItem_init(PttMenuItem* item) {
    item->label = furi_string_alloc();
    item->index = 0;
}

static void PttMenuItem_init_set(PttMenuItem* item, const PttMenuItem* src) {
    item->label = furi_string_alloc_set(src->label);
    item->index = src->index;
}

static void PttMenuItem_set(PttMenuItem* item, const PttMenuItem* src) {
    furi_string_set(item->label, src->label);
    item->index = src->index;
}

static void PttMenuItem_clear(PttMenuItem* item) {
    furi_string_free(item->label);
}

ARRAY_DEF(
    PttMenuItemArray,
    PttMenuItem,
    (INIT(API_2(PttMenuItem_init)),
     SET(API_6(PttMenuItem_set)),
     INIT_SET(API_6(PttMenuItem_init_set)),
     CLEAR(API_2(PttMenuItem_clear))))

// Main view
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
    // uint32_t appIndex;
    size_t appIndex;
    size_t window_position;
    PttMenuItemArray_t items;
    HidTransport transport;
} HidPttModel;

enum HidPttAppIndex {
    HidPttAppIndexGoogleMeet,
    HidPttAppIndexZoom,
    HidPttAppIndexFaceTime,
    HidPttAppIndexSkype,
    HidPttAppIndexSize,
};

static void hid_ptt_change_os(HidPtt* hid_ptt, bool vibro) {
    with_view_model(
        // hid_ptt->menuView,
        hid_ptt->view,
        HidPttModel * model,
        if (model->appIndex != HidPttAppIndexFaceTime) {
            model->is_mac_os = !model->is_mac_os;
            if (vibro){
                notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
            }
        },
    true);
}

// Menu

static void hid_ptt_menu_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidPttModel* model = context;
    const uint8_t item_height = 16;
    uint8_t item_width = canvas_width(canvas) - 5;

    canvas_clear(canvas);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 11, "<");
    canvas_draw_str(canvas, 32, 11, model->is_mac_os ? "             Mac" : "Windows/Linux");
    canvas_draw_str(canvas, 121, 11, ">");

    size_t position = 0;
    PttMenuItemArray_it_t it;
    for(PttMenuItemArray_it(it, model->items); !PttMenuItemArray_end_p(it);
        PttMenuItemArray_next(it)) {
        const size_t item_position = position - model->window_position;
        const size_t items_on_screen = 3;
        uint8_t y_offset = 16;

        if(item_position < items_on_screen) {
            if(position == model->appIndex) {
                canvas_set_color(canvas, ColorBlack);
                elements_slightly_rounded_box(
                    canvas,
                    0,
                    y_offset + (item_position * item_height) + 1,
                    item_width,
                    item_height - 2);
                canvas_set_color(canvas, ColorWhite);
            } else {
                canvas_set_color(canvas, ColorBlack);
            }

            FuriString* disp_str;
            disp_str = furi_string_alloc_set(PttMenuItemArray_cref(it)->label);
            elements_string_fit_width(canvas, disp_str, item_width - (6 * 2));

            canvas_draw_str(
                canvas,
                6,
                y_offset + (item_position * item_height) + item_height - 4,
                furi_string_get_cstr(disp_str));

            furi_string_free(disp_str);
        }

        position++;
    }
    elements_scrollbar_pos(canvas, 128 , 15, 49, model->appIndex, HidPttAppIndexSize);
}

void pttmenu_add_item(
    HidPtt* hid_ptt,
    const char* label,
    uint32_t index) {
    PttMenuItem* item = NULL;
    furi_assert(label);
    furi_assert(hid_ptt);

    with_view_model(
        hid_ptt->menuView, HidPttModel * model,
        {
            item = PttMenuItemArray_push_new(model->items);
            furi_string_set_str(item->label, label);
            item->index = index;
        },
        true);
}

// Main view

static void hid_ptt_draw_camera(Canvas* canvas, uint8_t x, uint8_t y) {
    canvas_draw_icon(canvas, x + 7, y, &I_ButtonLeft_4x7);
    canvas_draw_box(canvas, x, y, 7, 7);
}

static void hid_ptt_draw_cross(Canvas* canvas, uint8_t x, uint8_t y, uint8_t size, uint8_t thick) {
    uint8_t i = 0;
    for (i = 0; i < thick; i++) {
        canvas_draw_line(canvas, x + i, y       , x + size + i, y + size);
        canvas_draw_line(canvas, x + i, y + size, x + size + i, y);
    }
    // // canvas_draw_line(canvas, x + 1, y       , x + size + 1, y + size);
    // canvas_draw_line(canvas, x    , y       , x + size    , y + size);
    // // canvas_draw_line(canvas, x + 1, y + size, x + size + 1, y);
    // canvas_draw_line(canvas, x    , y + size, x + size    , y);
}

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
    if(model->appIndex == HidPttAppIndexGoogleMeet) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Google Meet");
    } else if(model->appIndex == HidPttAppIndexZoom) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Zoom");
    } else if(model->appIndex == HidPttAppIndexFaceTime) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "FaceTime");
    } else if(model->appIndex == HidPttAppIndexSkype) {
        elements_multiline_text_aligned(canvas, 7, y_app, AlignLeft, AlignTop, "Skype");
    }

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
    // const uint8_t y_mic = 102;
    // canvas_draw_icon(canvas, 18, y_mic - 1, &I_Ok_btn_9x9);
    // elements_multiline_text_aligned(canvas, 11, y_mic, AlignLeft, AlignTop, "+      to sync");
    // elements_multiline_text_aligned(canvas, 20, y_mic+10, AlignLeft, AlignTop, "mic status");

    // Exit label
    canvas_draw_icon(canvas, 18, 120, &I_Pin_back_arrow_rotated_8x10);
    elements_multiline_text_aligned(canvas, 0, 121, AlignLeft, AlignTop, "Hold    to exit");
    // elements_multiline_text_aligned(canvas, 0, 121, AlignLeft, AlignTop, "linux google meet");

    const uint8_t x_1 = 0;
    const uint8_t x_2 = x_1 + 19 + 4;
    const uint8_t x_3 = x_1 + 19 * 2 + 8;

    const uint8_t y_1 = 10;
    const uint8_t y_2 = y_1 + 19;
    const uint8_t y_3 = y_2 + 19;
    
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
    if (model->appIndex != HidPttAppIndexFaceTime && model->appIndex != HidPttAppIndexSkype) {
        canvas_draw_icon(canvas, x_1 + 4, y_2 + 3, &I_Hand_8x10);
    }
    canvas_set_color(canvas, ColorBlack);

    // Right / Camera
    canvas_draw_icon(canvas, x_3, y_2, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, x_3 + 3, y_2 + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    if (model->appIndex != HidPttAppIndexFaceTime) {
        hid_ptt_draw_camera(canvas, x_3 + 4, y_2 + 5);
        // canvas_draw_icon(canvas, x_3 + 11, y_2 + 5, &I_ButtonLeft_4x7);
        // canvas_draw_box(canvas, x_3 + 4, y_2 + 5, 7, 7);
    }
    canvas_set_color(canvas, ColorBlack);

    
    // Back / Mic
    const uint8_t x_mic = x_3;
    canvas_draw_icon(canvas, x_mic, 0, &I_Button_18x18);
    // canvas_draw_icon(canvas, x_mic + 2, 0, &I_Button_Round_16x16);
    if(model->mic_pressed) {
        elements_slightly_rounded_box(canvas, x_mic + 3, 2, 13, 13);
        // canvas_draw_icon(canvas, x_mic + 4, 2, &I_Pressed_Button_13x13);
        canvas_set_color(canvas, ColorWhite);
    }
    // canvas_draw_icon(canvas, x_mic + 6, 3, &I_Mic_btn_8x10);
    canvas_draw_icon(canvas, x_mic + 5, 4, &I_Mic_btn_8x10);
    
    if (!(!model->muted || (model->ptt_pressed))) {
        // hid_ptt_draw_cross(canvas, x_mic + 4, 3, 11, 1);
        hid_ptt_draw_cross(canvas, x_mic + 2, 2, 13, 1);
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


    if (model->ptt_pressed) {
        elements_slightly_rounded_box(canvas, x_ptt + 3, y_2 + 2, x_ptt_width + x_ptt_margin, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, x_ptt + 2 + x_ptt_margin / 2, y_2 + 13, AlignLeft, AlignBottom, "PTT");
    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
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

static void hid_ptt_trigger_hand(HidPtt* hid_ptt, HidPttModel * model) {
    if(model->appIndex == HidPttAppIndexGoogleMeet && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI | KEY_MOD_LEFT_CTRL |HID_KEYBOARD_H);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI | KEY_MOD_LEFT_CTRL |HID_KEYBOARD_H);
    } else if(model->appIndex == HidPttAppIndexGoogleMeet && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_CTRL | KEY_MOD_LEFT_ALT |HID_KEYBOARD_H);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_CTRL | KEY_MOD_LEFT_ALT |HID_KEYBOARD_H);
    } else if(model->appIndex == HidPttAppIndexZoom && model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_GUI| HID_KEYBOARD_Y);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_GUI| HID_KEYBOARD_Y );
    } else if(model->appIndex == HidPttAppIndexZoom && !model->is_mac_os) {
        hid_hal_keyboard_press(  hid_ptt->hid, KEY_MOD_LEFT_ALT | HID_KEYBOARD_Y);
        hid_hal_keyboard_release(hid_ptt->hid, KEY_MOD_LEFT_ALT | HID_KEYBOARD_Y );
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
            if(event->type == InputTypePress && !model->ptt_pressed) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    if (!model->mic_pressed){
                        hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                   } else {
                       hid_ptt_change_os(hid_ptt, true);
                   }
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    if (!model->mic_pressed){
                        hid_hal_consumer_key_press(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                    } else if (!model->mic_pressed) {
                        hid_ptt_shift_app(model, - 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    if (model->mic_pressed){
                        hid_ptt_shift_app(model, 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    if (model->mic_pressed){
                        hid_ptt_shift_app(model, - 1);
                        notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
                    }
                } else if(event->key == InputKeyOk) {
                    model->ptt_pressed = true;
                    if (!model->mic_pressed && model->muted){
                        hid_ptt_start_ptt(hid_ptt, model);
                    }
                } else if(event->key == InputKeyBack) {
                    model->mic_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                    if (!model->mic_pressed && !model->ptt_pressed){
                        hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_INCREMENT);
                    }
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                    if (!model->mic_pressed && !model->ptt_pressed){
                        hid_hal_consumer_key_release(hid_ptt->hid, HID_CONSUMER_VOLUME_DECREMENT);
                    }
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;

                } else if(event->key == InputKeyOk) {
                    model->ptt_pressed = false;
                    if(!model->mic_pressed) {
                        if (model->muted) {
                            hid_ptt_stop_ptt(hid_ptt, model);
                        } else {
                            hid_ptt_trigger_mute(hid_ptt, model);
                            model->muted = true;
                        }
                    }
                } else if(event->key == InputKeyBack) {
                    model->mic_pressed = false;
                }
            } else if(event->type == InputTypeShort && !model->ptt_pressed) {
                if(event->key == InputKeyBack ) { // no changes if PTT is pressed
                    model->muted = !model->muted;
                    hid_ptt_trigger_mute(hid_ptt, model);
                } else if(event->key == InputKeyRight) {
                    if (!model->mic_pressed){
                        hid_ptt_trigger_camera(hid_ptt, model);
                    }
                } else if(event->key == InputKeyLeft) {
                    if (!model->mic_pressed){
                        hid_ptt_trigger_hand(hid_ptt, model);
                    }
                }
            } else if(event->type == InputTypeLong && event->key == InputKeyRight) {
                model->muted = !model->muted;
                notification_message(hid_ptt->hid->notifications, &sequence_single_vibro);
            }
            //LED
            if (!model->muted || (model->ptt_pressed)) {
                notification_message(hid_ptt->hid->notifications, &sequence_set_red_255);
            } else {
                notification_message(hid_ptt->hid->notifications, &sequence_reset_red);
            }
        },
        true);
}

static bool hid_ptt_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidPtt* hid_ptt = context;
    bool consumed = false;
    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        hid_hal_keyboard_release_all(hid_ptt->hid);
        notification_message(hid_ptt->hid->notifications, &sequence_double_vibro);
    } else {
        consumed = true;
        hid_ptt_process(hid_ptt, event);
    }
    return consumed;
}

// Menu

void ptt_menu_process_up(HidPtt* hid_ptt) {
    with_view_model(
        hid_ptt->menuView, HidPttModel * model,
        {
            const size_t items_on_screen = 3;
            const size_t items_size = PttMenuItemArray_size(model->items);

            if(model->appIndex > 0) {
                model->appIndex--;
                if((model->appIndex == model->window_position) && (model->window_position > 0)) {
                    model->window_position--;
                }
            } else {
                model->appIndex = items_size - 1;
                if(model->appIndex > items_on_screen - 1) {
                    model->window_position = model->appIndex - (items_on_screen - 1);
                }
            }
        },
        true);
}

void ptt_menu_process_down(HidPtt* hid_ptt) {
    with_view_model(
        hid_ptt->menuView, HidPttModel * model,
        {
            const size_t items_on_screen = 3;
            const size_t items_size = PttMenuItemArray_size(model->items);

            if(model->appIndex < items_size - 1) {
                model->appIndex++;
                if((model->appIndex - model->window_position > items_on_screen - 2) &&
                   (model->window_position < items_size - items_on_screen)) {
                    model->window_position++;
                }
            } else {
                model->appIndex = 0;
                model->window_position = 0;
            }
        },
        true);
}

void ptt_menu_process_ok(HidPtt* hid_ptt) {
    // PttMenuItem* item = NULL;

    // with_view_model(
    //     hid_ptt->view, HidPttModel * model,
    //     {
    //         const size_t items_size = PttMenuItemArray_size(model->items);
    //         if(model->appIndex < items_size) {
    //             item = PttMenuItemArray_get(model->items, model->appIndex);
    //         }
    //     },
    //     true);
    // view_set_draw_callback(hid_ptt->view, hid_ptt_draw_callback);
    // view_set_input_callback(hid_ptt->view, hid_ptt_input_callback);
    // view_set_orientation(hid_ptt->view, ViewOrientationVerticalFlip);
    view_dispatcher_switch_to_view(hid_ptt->hid->view_dispatcher, HidViewPushToTalk);
    // view_dispatcher_switch_to_view(hid_ptt->hid->view_dispatcher, HidViewMovie);
}

static bool hid_ptt_menu_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidPtt* hid_ptt = context;
    bool consumed = false;
    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            consumed = true;
            ptt_menu_process_up(hid_ptt);
            break;
        case InputKeyDown:
            consumed = true;
            ptt_menu_process_down(hid_ptt);
            break;
        case InputKeyLeft:
            consumed = true;
            hid_ptt_change_os(hid_ptt, false);
            break;
        case InputKeyRight:
            consumed = true;
            hid_ptt_change_os(hid_ptt, false);
            break;
        case InputKeyOk:
            consumed = true;
            ptt_menu_process_ok(hid_ptt);
            break;
        default:
            break;
        }
    } else if(event->type == InputTypeRepeat) {
        if(event->key == InputKeyUp) {
            consumed = true;
            ptt_menu_process_up(hid_ptt);
        } else if(event->key == InputKeyDown) {
            consumed = true;
            ptt_menu_process_down(hid_ptt);
        }
    }
    return consumed;
}

// Main view

View* hid_ptt_get_view(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    return hid_ptt->view;
}

View* hid_ptt_get_menu_view(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    return hid_ptt->menuView;
}


HidPtt* hid_ptt_alloc(Hid* hid) {
    HidPtt* hid_ptt = malloc(sizeof(HidPtt));
    hid_ptt->hid = hid;
    hid_ptt->menuView = view_alloc();
    view_set_context(hid_ptt->menuView, hid_ptt);
    view_allocate_model(hid_ptt->menuView, ViewModelTypeLocking, sizeof(HidPttModel));
    view_set_draw_callback(hid_ptt->menuView, hid_ptt_menu_draw_callback);
    view_set_input_callback(hid_ptt->menuView, hid_ptt_menu_input_callback);

    hid_ptt->view = view_alloc();
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
    // with_view_model(
    //     hid_ptt->menuView, HidPttModel * model, {
    //         model->transport = hid->transport;
    //         model->muted = true; // assume we're muted
    //         model->is_mac_os = true;
    //     }, true);
    
    pttmenu_add_item(hid_ptt, "Google Meet", HidPttAppIndexGoogleMeet);
    pttmenu_add_item(hid_ptt, "Zoom", HidPttAppIndexZoom);
    pttmenu_add_item(hid_ptt, "FaceTime", HidPttAppIndexFaceTime);
    pttmenu_add_item(hid_ptt, "Skype", HidPttAppIndexSkype);

    return hid_ptt;
}

void hid_ptt_free(HidPtt* hid_ptt) {
    furi_assert(hid_ptt);
    notification_message(hid_ptt->hid->notifications, &sequence_reset_red);
    view_free(hid_ptt->view);
    // view_free(hid_ptt->menuView);
    // view_dispatcher_remove_view(hid_ptt->hid->view_dispatcher, HidViewPushToTalk);
    free(hid_ptt);
}

void hid_ptt_set_connected_status(HidPtt* hid_ptt, bool connected) {
    furi_assert(hid_ptt);
    with_view_model(
        hid_ptt->view, HidPttModel * model, { model->connected = connected; }, true);
}
