#include "bt_hid_media.h"
#include <furi.h>
#include <furi-hal-bt-hid.h>
#include <furi-hal-usb-hid.h>
#include <gui/elements.h>

struct BtHidMedia {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
} BtHidMediaModel;

static void bt_hid_media_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    BtHidMediaModel* model = context;

    // Header
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 3, 0, AlignLeft, AlignTop, "Media player");
    canvas_set_font(canvas, FontSecondary);

    // Connected status
    canvas_draw_icon(canvas, 20, 24, &I_Medium_ble_12x21);
    if(model->connected) {
        canvas_draw_icon(canvas, 34, 28, &I_Waves_9x13);
        elements_multiline_text_aligned(canvas, 3, 62, AlignLeft, AlignBottom, "Connected");
    } else {
        canvas_draw_icon(canvas, 35, 30, &I_Cross_9x9);
        elements_multiline_text_aligned(canvas, 3, 62, AlignLeft, AlignBottom, "Disconnected");
    }

    // Keypad circles
    canvas_draw_circle(canvas, 96, 32, 31);
    canvas_draw_circle(canvas, 96, 32, 12);

    // Up
    if(model->up_pressed) {
        canvas_draw_disc(canvas, 96, 10, 8);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 91, 6, &I_Vol_up_12x9);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        canvas_draw_disc(canvas, 96, 54, 7);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 91, 51, &I_Vol_dwn_10x7);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        canvas_draw_disc(canvas, 75, 32, 7);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_arrow(canvas, 79, 32, 7, 4, CanvasDirectionRightToLeft);
    canvas_draw_arrow(canvas, 74, 32, 7, 4, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_draw_disc(canvas, 117, 32, 7);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_arrow(canvas, 113, 32, 7, 4, CanvasDirectionLeftToRight);
    canvas_draw_arrow(canvas, 118, 32, 7, 4, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    if(model->ok_pressed) {
        canvas_draw_disc(canvas, 96, 32, 8);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_arrow(canvas, 91, 32, 7, 4, CanvasDirectionLeftToRight);
    canvas_draw_box(canvas, 97, 29, 2, 7);
    canvas_draw_box(canvas, 100, 29, 2, 7);
    canvas_set_color(canvas, ColorBlack);
}

static void bt_hid_media_process_press(BtHidMedia* bt_hid_media, InputEvent* event) {
    with_view_model(
        bt_hid_media->view, (BtHidMediaModel * model) {
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

static void bt_hid_media_process_release(BtHidMedia* bt_hid_media, InputEvent* event) {
    with_view_model(
        bt_hid_media->view, (BtHidMediaModel * model) {
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

static bool bt_hid_media_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BtHidMedia* bt_hid_media = context;
    bool consumed = false;

    if(event->type == InputTypePress) {
        bt_hid_media_process_press(bt_hid_media, event);
        consumed = true;
    } else if(event->type == InputTypeRelease) {
        bt_hid_media_process_release(bt_hid_media, event);
        consumed = true;
    } else if(event->type == InputTypeShort) {
        consumed = true;
    }

    return consumed;
}

BtHidMedia* bt_hid_media_alloc() {
    BtHidMedia* bt_hid_media = furi_alloc(sizeof(BtHidMedia));
    bt_hid_media->view = view_alloc();
    view_set_context(bt_hid_media->view, bt_hid_media);
    view_allocate_model(bt_hid_media->view, ViewModelTypeLocking, sizeof(BtHidMediaModel));
    view_set_draw_callback(bt_hid_media->view, bt_hid_media_draw_callback);
    view_set_input_callback(bt_hid_media->view, bt_hid_media_input_callback);

    return bt_hid_media;
}

void bt_hid_media_free(BtHidMedia* bt_hid_media) {
    furi_assert(bt_hid_media);
    view_free(bt_hid_media->view);
    free(bt_hid_media);
}

View* bt_hid_media_get_view(BtHidMedia* bt_hid_media) {
    furi_assert(bt_hid_media);
    return bt_hid_media->view;
}

void bt_hid_media_set_connected_status(BtHidMedia* bt_hid_media, bool connected) {
    furi_assert(bt_hid_media);
    with_view_model(
        bt_hid_media->view, (BtHidMediaModel * model) {
            model->connected = connected;
            return true;
        });
}
