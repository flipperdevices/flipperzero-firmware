#include "xbox_controller_view.h"

struct XboxControllerView {
    View* view;
    NotificationApp* notifications;
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
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_triangle(canvas, x - 2, y - 2, 5, 3, dir);
        canvas_draw_line(canvas, x - 2, y - 3, x - 2, y + 4);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_triangle(canvas, x - 2, y + 2, 5, 3, dir);
        canvas_draw_line(canvas, x - 2, y - 4, x - 2, y + 3);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_triangle(canvas, x - 4, y, 5, 3, dir);
        canvas_draw_line(canvas, x + 2, y, x - 5, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_triangle(canvas, x, y, 5, 3, dir);
        canvas_draw_line(canvas, x - 6, y, x + 1, y);
    }
}

static void xbox_controller_view_draw_arrow_button(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    CanvasDirection direction) {
    canvas_draw_icon(canvas, x, y, &I_Button_18x18);
    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 3, y + 2, 13, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    xbox_controller_view_draw_arrow(canvas, x + 11, y + 8, direction);
    canvas_set_color(canvas, ColorBlack);
}

static void xbox_controller_draw_wide_button(
    Canvas* canvas,
    bool pressed,
    uint8_t x,
    uint8_t y,
    char* text,
    const Icon* icon) {
    // canvas_draw_icon(canvas, 0, 25, &I_Space_65x18);
    elements_slightly_rounded_frame(canvas, x, y, 64, 17);
    if(pressed) {
        elements_slightly_rounded_box(canvas, x + 2, y + 2, 60, 13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_icon(canvas, x + 11, y + 4, icon);
    elements_multiline_text_aligned(canvas, x + 28, y + 12, AlignLeft, AlignBottom, text);
    canvas_set_color(canvas, ColorBlack);
}

static void xbox_controller_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    XboxControllerViewModel* model = context;

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Xbox One");

    canvas_set_font(canvas, FontSecondary);

    canvas_draw_icon(canvas, 0, 12, &I_Pin_back_arrow_10x8);
    canvas_draw_str(canvas, 12, 20, "Hold");

    xbox_controller_view_draw_arrow_button(
        canvas, model->up_pressed, 23, 74, CanvasDirectionBottomToTop);
    xbox_controller_view_draw_arrow_button(
        canvas, model->down_pressed, 23, 110, CanvasDirectionTopToBottom);
    xbox_controller_view_draw_arrow_button(
        canvas, model->left_pressed, 0, 92, CanvasDirectionRightToLeft);
    xbox_controller_view_draw_arrow_button(
        canvas, model->right_pressed, 46, 92, CanvasDirectionLeftToRight);

    int buttons_post = 30;
    // Ok
    xbox_controller_draw_wide_button(
        canvas, model->ok_pressed, 0, buttons_post, "A", &I_Ok_btn_9x9);
    // Back
    xbox_controller_draw_wide_button(
        canvas, model->back_pressed, 0, buttons_post + 19, "B", &I_Pin_back_arrow_10x8);
}

const NotificationSequence sequence_blink_purple_50 = {
    &message_red_255,
    &message_blue_255,
    &message_delay_50,
    NULL,
};

void send_xbox_ir(uint32_t command, NotificationApp* notifications) {
    InfraredMessage* message = malloc(sizeof(InfraredMessage));
    message->protocol = InfraredProtocolNECext;
    message->address = 0xD880;
    message->command = command;
    message->repeat = false;
    notification_message(notifications, &sequence_blink_purple_50);
    infrared_send(message, 2);
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
                    send_xbox_ir(0xE11E, xbox_controller_view->notifications);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    send_xbox_ir(0xE01F, xbox_controller_view->notifications);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    send_xbox_ir(0xDF20, xbox_controller_view->notifications);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    send_xbox_ir(0xDE21, xbox_controller_view->notifications);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    send_xbox_ir(0x9966, xbox_controller_view->notifications);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                    send_xbox_ir(0x9A65, xbox_controller_view->notifications);
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

XboxControllerView* xbox_controller_view_alloc(NotificationApp* notifications) {
    XboxControllerView* xbox_controller_view = malloc(sizeof(XboxControllerView));
    xbox_controller_view->view = view_alloc();
    xbox_controller_view->notifications = notifications;
    view_set_orientation(xbox_controller_view->view, ViewOrientationVertical);
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
