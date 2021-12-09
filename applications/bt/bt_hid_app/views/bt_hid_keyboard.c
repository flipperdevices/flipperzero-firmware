#include "bt_hid_keyboard.h"
#include <furi.h>
#include <furi-hal-bt-hid.h>
#include <gui/elements.h>

struct BtHidKeyboard {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
} BtHidKeyboardModel;

static void bt_hid_keyboard_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    BtHidKeyboardModel* model = context;

    // Header
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 3, 2, AlignLeft, AlignTop, "BLE Presenter");
    canvas_set_font(canvas, FontSecondary);
    // Connected status
    if(model->connected) {
        canvas_draw_icon(canvas, 3, 15, &I_BLE_Connected_9x8);
        elements_multiline_text_aligned(canvas, 15, 16, AlignLeft, AlignTop, "Connected");
    } else {
        canvas_draw_icon(canvas, 3, 15, &I_BLE_Disconnected_9x8);
        elements_multiline_text_aligned(canvas, 15, 16, AlignLeft, AlignTop, "Disconnected");
    }

    // Up
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, 93, 12, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 93, 12, 17, 17);
    }
    canvas_draw_line(canvas, 97, 22, 105, 22);
    canvas_draw_line(canvas, 97, 21, 101, 17);
    canvas_draw_line(canvas, 101, 17, 105, 21);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, 93, 47, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 93, 47, 17, 17);
    }
    canvas_draw_line(canvas, 97, 53, 105, 53);
    canvas_draw_line(canvas, 97, 54, 101, 58);
    canvas_draw_line(canvas, 101, 58, 105, 54);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, 75, 30, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 75, 30, 17, 17);
    }
    canvas_draw_line(canvas, 85, 34, 85, 42);
    canvas_draw_line(canvas, 84, 34, 80, 38);
    canvas_draw_line(canvas, 80, 38, 84, 42);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, 111, 30, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 111, 30, 17, 17);
    }
    canvas_draw_line(canvas, 117, 34, 117, 42);
    canvas_draw_line(canvas, 118, 34, 122, 38);
    canvas_draw_line(canvas, 122, 38, 118, 42);
    canvas_set_color(canvas, ColorBlack);

    // Space
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 0, 47, 64, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 0, 47, 64, 17);
    }
    elements_multiline_text_aligned(canvas, 32, 58, AlignCenter, AlignBottom, "OK = Space");
}

static void bt_hid_keyboard_process_press(BtHidKeyboard* bt_hid_keyboard, InputEvent* event) {
    with_view_model(
        bt_hid_keyboard->view, (BtHidKeyboardModel * model) {
            if(event->key == InputKeyUp) {
                model->up_pressed = true;
                // furi_hal_bt_hid_kb_press();
            } else if(event->key == InputKeyDown) {
                model->down_pressed = true;
                // furi_hal_bt_hid_kb_press();
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = true;
                // furi_hal_bt_hid_kb_press();
            } else if(event->key == InputKeyRight) {
                model->right_pressed = true;
                // furi_hal_bt_hid_kb_press();
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = true;
                // furi_hal_bt_hid_kb_press();
            }
            return true;
        });
}

static void bt_hid_keyboard_process_release(BtHidKeyboard* bt_hid_keyboard, InputEvent* event) {
    with_view_model(
        bt_hid_keyboard->view, (BtHidKeyboardModel * model) {
            if(event->key == InputKeyUp) {
                model->up_pressed = false;
                // furi_hal_bt_hid_kb_release();
            } else if(event->key == InputKeyDown) {
                model->down_pressed = false;
                // furi_hal_bt_hid_kb_release();
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = false;
                // furi_hal_bt_hid_kb_release();
            } else if(event->key == InputKeyRight) {
                model->right_pressed = false;
                // furi_hal_bt_hid_kb_release();
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = false;
                // furi_hal_bt_hid_kb_release();
            }
            return true;
        });
}

static bool bt_hid_keyboard_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BtHidKeyboard* bt_hid_keyboard = context;
    bool consumed = false;

    if(event->type == InputTypePress) {
        bt_hid_keyboard_process_press(bt_hid_keyboard, event);
        consumed = true;
    } else if(event->type == InputTypeRelease) {
        bt_hid_keyboard_process_release(bt_hid_keyboard, event);
        consumed = true;
    } else if(event->type == InputTypeShort) {
        consumed = true;
    }

    return consumed;
}

BtHidKeyboard* bt_hid_keyboard_alloc() {
    BtHidKeyboard* bt_hid_keyboard = furi_alloc(sizeof(BtHidKeyboard));
    bt_hid_keyboard->view = view_alloc();
    view_set_context(bt_hid_keyboard->view, bt_hid_keyboard);
    view_allocate_model(bt_hid_keyboard->view, ViewModelTypeLocking, sizeof(BtHidKeyboardModel));
    view_set_draw_callback(bt_hid_keyboard->view, bt_hid_keyboard_draw_callback);
    view_set_input_callback(bt_hid_keyboard->view, bt_hid_keyboard_input_callback);

    return bt_hid_keyboard;
}

void bt_hid_keyboard_free(BtHidKeyboard* bt_hid_keyboard) {
    furi_assert(bt_hid_keyboard);
    view_free(bt_hid_keyboard->view);
    free(bt_hid_keyboard);
}

View* bt_hid_keyboard_get_view(BtHidKeyboard* bt_hid_keyboard) {
    furi_assert(bt_hid_keyboard);
    return bt_hid_keyboard->view;
}
