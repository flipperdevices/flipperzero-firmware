#include "xbox_controller_view.h"
#include <furi.h>
#include <gui/elements.h>
#include <xc_icons.h>

#include <infrared_transmit.h>

struct XboxControllerView {
    View* view;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool back_pressed;
    bool connected;
} XboxControllerViewModel;

static void
    xbox_controller_view_draw_arrow(Canvas* canvas, uint8_t x, uint8_t y, CanvasDirection dir) {
    canvas_draw_triangle(canvas, x, y, 5, 3, dir);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_line(canvas, x, y + 6, x, y - 1);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_line(canvas, x, y - 6, x, y + 1);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_line(canvas, x + 6, y, x - 1, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    }
}

static void xbox_controller_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XboxControllerViewModel* model = context;

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Xbox One");

    canvas_draw_icon(canvas, 54, 0, &I_Pin_back_arrow_10x8);
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 127, 1, AlignRight, AlignTop, "Hold to change");

    // Up
    canvas_draw_icon(canvas, 21, 24, &I_Button_18x18);
    if(model->up_pressed) {
        elements_slightly_rounded_box(canvas, 24, 26, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    xbox_controller_view_draw_arrow(canvas, 30, 30, CanvasDirectionBottomToTop);
    canvas_set_color(canvas, ColorBlack);

    // Down
    canvas_draw_icon(canvas, 21, 45, &I_Button_18x18);
    if(model->down_pressed) {
        elements_slightly_rounded_box(canvas, 24, 47, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    xbox_controller_view_draw_arrow(canvas, 30, 55, CanvasDirectionTopToBottom);
    canvas_set_color(canvas, ColorBlack);

    // Left
    canvas_draw_icon(canvas, 0, 45, &I_Button_18x18);
    if(model->left_pressed) {
        elements_slightly_rounded_box(canvas, 3, 47, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    xbox_controller_view_draw_arrow(canvas, 7, 53, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    canvas_draw_icon(canvas, 42, 45, &I_Button_18x18);
    if(model->right_pressed) {
        elements_slightly_rounded_box(canvas, 45, 47, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    xbox_controller_view_draw_arrow(canvas, 53, 53, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Ok
    canvas_draw_icon(canvas, 63, 25, &I_Space_65x18);
    if(model->ok_pressed) {
        elements_slightly_rounded_box(canvas, 66, 27, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 74, 29, &I_Ok_btn_9x9);
    elements_multiline_text_aligned(canvas, 91, 36, AlignLeft, AlignBottom, "A");
    canvas_set_color(canvas, ColorBlack);

    // Back
    canvas_draw_icon(canvas, 63, 45, &I_Space_65x18);
    if(model->back_pressed) {
        elements_slightly_rounded_box(canvas, 66, 47, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, 74, 49, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 91, 57, AlignLeft, AlignBottom, "B");
}

void send_xbox_ir(uint32_t command) {
    InfraredMessage* message = malloc(sizeof(InfraredMessage));
    message->protocol = InfraredProtocolNECext;
    message->address = 0x80D80000;
    message->command = command;
    message->repeat = false;
    infrared_send(message, 1);
    free(message);
}

static void
    xbox_controller_view_process(XboxControllerView* xbox_controller_view, InputEvent* event) {
    with_view_model(
        xbox_controller_view->view,
        XboxControllerViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    send_xbox_ir(0x1EE10000);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    send_xbox_ir(0x1FE00000);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    send_xbox_ir(0x20DF0000);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    send_xbox_ir(0x21DE0000);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    send_xbox_ir(0x66990000);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                    send_xbox_ir(0x659A0000);
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = false;
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                    // furi_hal_hid_kb_press(HID_KEYBOARD_DELETE);
                    // furi_hal_hid_kb_release(HID_KEYBOARD_DELETE);
                    // furi_hal_hid_consumer_key_press(HID_CONSUMER_AC_BACK);
                    // furi_hal_hid_consumer_key_release(HID_CONSUMER_AC_BACK);
                }
            }
        },
        true);
}

static bool xbox_controller_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    XboxControllerView* xbox_controller_view = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        // LONG KEY BACK PRESS HANDLER
    } else {
        xbox_controller_view_process(xbox_controller_view, event);
        consumed = true;
    }

    return consumed;
}

XboxControllerView* xbox_controller_view_alloc() {
    XboxControllerView* xbox_controller_view = malloc(sizeof(XboxControllerView));
    xbox_controller_view->view = view_alloc();
    view_set_context(xbox_controller_view->view, xbox_controller_view);
    view_allocate_model(
        xbox_controller_view->view, ViewModelTypeLocking, sizeof(XboxControllerViewModel));
    view_set_draw_callback(xbox_controller_view->view, xbox_controller_view_draw_callback);
    view_set_input_callback(xbox_controller_view->view, xbox_controller_view_input_callback);

    return xbox_controller_view;
}

void xbox_controller_view_free(XboxControllerView* xbox_controller_view) {
    furi_assert(xbox_controller_view);
    view_free(xbox_controller_view->view);
    free(xbox_controller_view);
}

View* xbox_controller_view_get_view(XboxControllerView* xbox_controller_view) {
    furi_assert(xbox_controller_view);
    return xbox_controller_view->view;
}

void xbox_controller_view_set_connected_status(
    XboxControllerView* xbox_controller_view,
    bool connected) {
    furi_assert(xbox_controller_view);
    with_view_model(
        xbox_controller_view->view,
        XboxControllerViewModel * model,
        { model->connected = connected; },
        true);
}
