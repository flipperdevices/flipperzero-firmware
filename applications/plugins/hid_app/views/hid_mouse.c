#include "bt_hid_mouse.h"
#include <furi.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidApp"

struct HidMouse {
    View* view;
    Hid* hid;
};
#define MOUSE_MOVE_SHORT 5
#define MOUSE_MOVE_LONG 20

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool left_mouse_pressed;
    bool left_mouse_held;
    bool right_mouse_pressed;
    bool connected;
    bool is_bluetooth;
} HidMouseModel;

static void hid_mouse_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidMouseModel* model = context;
    bool is_bluetooth = model->is_bluetooth;

    // Header
    if(model->connected && is_bluetooth) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Mouse");
    canvas_set_font(canvas, FontSecondary);

    if(model->left_mouse_held == true) {
        elements_multiline_text_aligned(canvas, 0, 62, AlignLeft, AlignBottom, "Selecting...");
    } else {
        canvas_draw_icon(canvas, 0, 54, &I_Pin_back_arrow_10x8);
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
    }

    // Keypad circles
    canvas_draw_icon(canvas, 64, 8, &I_Circles_47x47);

    // Up
    if(model->up_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 81, 9, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 84, 10, &I_Pin_arrow_up_7x9);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 81, 41, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 84, 43, &I_Pin_arrow_down_7x9);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 65, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 67, 28, &I_Pin_arrow_left_9x7);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 97, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 99, 28, &I_Pin_arrow_right_9x7);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->left_mouse_pressed) {
        canvas_draw_icon(canvas, 81, 25, &I_Ok_btn_pressed_13x13);
    } else {
        canvas_draw_icon(canvas, 83, 27, &I_Left_mouse_icon_9x9);
    }

    // Back
    if(model->right_mouse_pressed) {
        canvas_draw_icon(canvas, 108, 48, &I_Ok_btn_pressed_13x13);
    } else {
        canvas_draw_icon(canvas, 110, 50, &I_Right_mouse_icon_9x9);
    }
}

static void hid_mouse_process(HidMouse* hid_mouse, InputEvent* event) {
    bool is_bluetooth = hid_mouse->hid->is_bluetooth;
    with_view_model(
        hid_mouse->view,
        HidMouseModel * model,
        {
            if(event->key == InputKeyBack) {
                if(event->type == InputTypeShort) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_press(HID_MOUSE_BTN_RIGHT);
                        furi_hal_bt_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_press(HID_MOUSE_BTN_RIGHT);
                        furi_hal_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
                    }
                } else if(event->type == InputTypePress) {
                    model->right_mouse_pressed = true;
                } else if(event->type == InputTypeRelease) {
                    model->right_mouse_pressed = false;
                }
            } else if(event->key == InputKeyOk) {
                if(event->type == InputTypeShort) {
                    // Just release if it was being held before
                    if(is_bluetooth) {
                        if(!model->left_mouse_held)
                            furi_hal_bt_hid_mouse_press(HID_MOUSE_BTN_LEFT);
                        furi_hal_bt_hid_mouse_release(HID_MOUSE_BTN_LEFT);
                    } else if(!is_bluetooth) {
                        if(!model->left_mouse_held) furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
                        furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
                    }
                    model->left_mouse_held = false;
                } else if(event->type == InputTypeLong) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_press(HID_MOUSE_BTN_LEFT);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
                    }
                    model->left_mouse_held = true;
                    model->left_mouse_pressed = true;
                } else if(event->type == InputTypePress) {
                    model->left_mouse_pressed = true;
                } else if(event->type == InputTypeRelease) {
                    // Only release if it wasn't a long press
                    if(!model->left_mouse_held) model->left_mouse_pressed = false;
                }

            } else if(event->key == InputKeyRight) {
                if(event->type == InputTypePress) {
                    model->right_pressed = true;
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(MOUSE_MOVE_SHORT, 0);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(MOUSE_MOVE_SHORT, 0);
                    }
                } else if(event->type == InputTypeRepeat) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(MOUSE_MOVE_LONG, 0);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(MOUSE_MOVE_LONG, 0);
                    }
                } else if(event->type == InputTypeRelease) {
                    model->right_pressed = false;
                }
            } else if(event->key == InputKeyLeft) {
                if(event->type == InputTypePress) {
                    model->left_pressed = true;
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(-MOUSE_MOVE_SHORT, 0);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(-MOUSE_MOVE_SHORT, 0);
                    }
                } else if(event->type == InputTypeRepeat) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(-MOUSE_MOVE_LONG, 0);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(-MOUSE_MOVE_LONG, 0);
                    }
                } else if(event->type == InputTypeRelease) {
                    model->left_pressed = false;
                }
            } else if(event->key == InputKeyDown) {
                if(event->type == InputTypePress) {
                    model->down_pressed = true;
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(0, MOUSE_MOVE_SHORT);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(0, MOUSE_MOVE_SHORT);
                    }
                } else if(event->type == InputTypeRepeat) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(0, MOUSE_MOVE_LONG);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(0, MOUSE_MOVE_LONG);
                    }
                } else if(event->type == InputTypeRelease) {
                    model->down_pressed = false;
                }
            } else if(event->key == InputKeyUp) {
                if(event->type == InputTypePress) {
                    model->up_pressed = true;
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(0, -MOUSE_MOVE_SHORT);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(0, -MOUSE_MOVE_SHORT);
                    }
                } else if(event->type == InputTypeRepeat) {
                    if(is_bluetooth) {
                        furi_hal_bt_hid_mouse_move(0, -MOUSE_MOVE_LONG);
                    } else if(!is_bluetooth) {
                        furi_hal_hid_mouse_move(0, -MOUSE_MOVE_LONG);
                    }
                } else if(event->type == InputTypeRelease) {
                    model->up_pressed = false;
                }
            }
        },
        true);
}

static bool hid_mouse_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidMouse* hid_mouse = context;
    bool is_bluetooth = hid_mouse->hid->is_bluetooth;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        if(is_bluetooth) {
            furi_hal_bt_hid_mouse_release_all();
        } else if(!is_bluetooth) {
            furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
            furi_hal_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
        }
    } else {
        hid_mouse_process(hid_mouse, event);
        consumed = true;
    }

    return consumed;
}

HidMouse* hid_mouse_alloc(Hid* hid) {
    HidMouse* hid_mouse = malloc(sizeof(HidMouse));
    hid_mouse->view = view_alloc();
    hid_mouse->hid = hid;
    view_set_context(hid_mouse->view, hid_mouse);
    view_allocate_model(hid_mouse->view, ViewModelTypeLocking, sizeof(HidMouseModel));
    view_set_draw_callback(hid_mouse->view, hid_mouse_draw_callback);
    view_set_input_callback(hid_mouse->view, hid_mouse_input_callback);

    return hid_mouse;
}

void hid_mouse_free(HidMouse* hid_mouse) {
    furi_assert(hid_mouse);
    view_free(hid_mouse->view);
    free(hid_mouse);
}

View* hid_mouse_get_view(HidMouse* hid_mouse) {
    furi_assert(hid_mouse);
    return hid_mouse->view;
}

void hid_mouse_set_connected_status(HidMouse* hid_mouse, bool connected) {
    furi_assert(hid_mouse);
    with_view_model(
        hid_mouse->view, HidMouseModel * model, { model->connected = connected; }, true);
}

void hid_mouse_set_conn_type(HidMouse* hid_mouse, bool is_bluetooth) {
    furi_assert(hid_mouse);
    with_view_model(
        hid_mouse->view, HidMouseModel * model, { model->is_bluetooth = is_bluetooth; }, true);
}
