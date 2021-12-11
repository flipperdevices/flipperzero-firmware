#include "bt_hid_keynote.h"
#include <furi.h>
#include <furi-hal-bt-hid.h>
#include <furi-hal-usb-hid.h>
#include <gui/elements.h>

struct BtHidKeynote {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
} BtHidKeynoteModel;

static void bt_hid_keynote_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    BtHidKeynoteModel* model = context;

    // Header
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 3, 2, AlignLeft, AlignTop, "Keynote");
    canvas_set_font(canvas, FontSecondary);

    // Connected status
    canvas_draw_icon(canvas, 22, 24, &I_Medium_ble_12x21);
    if(model->connected) {
        canvas_draw_icon(canvas, 36, 28, &I_Waves_9x13);
        elements_multiline_text_aligned(canvas, 3, 13, AlignLeft, AlignTop, "Connected");
    } else {
        canvas_draw_icon(canvas, 37, 30, &I_Cross_9x9);
        elements_multiline_text_aligned(canvas, 3, 13, AlignLeft, AlignTop, "Disconnected");
    }

    // Up
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, 90, 26, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 90, 26, 17, 17);
    }
    canvas_draw_arrow(canvas, 98, 36, 9, 5, CanvasDirectionDownToTop);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, 90, 47, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 90, 47, 17, 17);
    }
    canvas_draw_arrow(canvas, 98, 53, 9, 5, CanvasDirectionTopToDown);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, 69, 47, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 69, 47, 17, 17);
    }
    canvas_draw_arrow(canvas, 79, 55, 9, 5, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, 111, 47, 17, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 111, 47, 17, 17);
    }
    canvas_draw_arrow(canvas, 117, 55, 9, 5, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 0, 47, 64, 17);
        canvas_set_color(canvas, ColorWhite);
    } else {
        elements_slightly_rounded_frame(canvas, 0, 47, 64, 17);
    }
    elements_multiline_text_aligned(canvas, 32, 58, AlignCenter, AlignBottom, "OK = Space");
}

static void bt_hid_keynote_process_press(BtHidKeynote* bt_hid_keynote, InputEvent* event) {
    with_view_model(
        bt_hid_keynote->view, (BtHidKeynoteModel * model) {
            if(event->key == InputKeyUp) {
                model->up_pressed = true;
                furi_hal_bt_hid_kb_press(KEY_UP_ARROW);
            } else if(event->key == InputKeyDown) {
                model->down_pressed = true;
                furi_hal_bt_hid_kb_press(KEY_DOWN_ARROW);
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = true;
                furi_hal_bt_hid_kb_press(KEY_LEFT_ARROW);
            } else if(event->key == InputKeyRight) {
                model->right_pressed = true;
                furi_hal_bt_hid_kb_press(KEY_RIGHT_ARROW);
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = true;
                furi_hal_bt_hid_kb_press(KEY_SPACE);
            }
            return true;
        });
}

static void bt_hid_keynote_process_release(BtHidKeynote* bt_hid_keynote, InputEvent* event) {
    with_view_model(
        bt_hid_keynote->view, (BtHidKeynoteModel * model) {
            if(event->key == InputKeyUp) {
                model->up_pressed = false;
                furi_hal_bt_hid_kb_release(KEY_UP_ARROW);
            } else if(event->key == InputKeyDown) {
                model->down_pressed = false;
                furi_hal_bt_hid_kb_release(KEY_DOWN_ARROW);
            } else if(event->key == InputKeyLeft) {
                model->left_pressed = false;
                furi_hal_bt_hid_kb_release(KEY_LEFT_ARROW);
            } else if(event->key == InputKeyRight) {
                model->right_pressed = false;
                furi_hal_bt_hid_kb_release(KEY_RIGHT_ARROW);
            } else if(event->key == InputKeyOk) {
                model->ok_pressed = false;
                furi_hal_bt_hid_kb_release(KEY_SPACE);
            }
            return true;
        });
}

static bool bt_hid_keynote_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BtHidKeynote* bt_hid_keynote = context;
    bool consumed = false;

    if(event->type == InputTypePress) {
        bt_hid_keynote_process_press(bt_hid_keynote, event);
        consumed = true;
    } else if(event->type == InputTypeRelease) {
        bt_hid_keynote_process_release(bt_hid_keynote, event);
        consumed = true;
    } else if(event->type == InputTypeShort) {
        consumed = true;
    }

    return consumed;
}

BtHidKeynote* bt_hid_keynote_alloc() {
    BtHidKeynote* bt_hid_keynote = furi_alloc(sizeof(BtHidKeynote));
    bt_hid_keynote->view = view_alloc();
    view_set_context(bt_hid_keynote->view, bt_hid_keynote);
    view_allocate_model(bt_hid_keynote->view, ViewModelTypeLocking, sizeof(BtHidKeynoteModel));
    view_set_draw_callback(bt_hid_keynote->view, bt_hid_keynote_draw_callback);
    view_set_input_callback(bt_hid_keynote->view, bt_hid_keynote_input_callback);

    return bt_hid_keynote;
}

void bt_hid_keynote_free(BtHidKeynote* bt_hid_keynote) {
    furi_assert(bt_hid_keynote);
    view_free(bt_hid_keynote->view);
    free(bt_hid_keynote);
}

View* bt_hid_keynote_get_view(BtHidKeynote* bt_hid_keynote) {
    furi_assert(bt_hid_keynote);
    return bt_hid_keynote->view;
}

void bt_hid_keynote_set_connected_status(BtHidKeynote* bt_hid_keynote, bool connected) {
    furi_assert(bt_hid_keynote);
    with_view_model(
        bt_hid_keynote->view, (BtHidKeynoteModel * model) {
            model->connected = connected;
            return true;
        });
}
