#include "reset_view.h"
#include <furi.h>
#include <gui/elements.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <furi_hal_uart.h>

#define MODULE_CONTROL_COMMAND_RESET 'a'
#define FLIPPERZERO_SERIAL_BAUD 115200

typedef enum ESerialCommand { ESerialCommand_Reset } ESerialCommand;

struct ResetView {
    View* view;
};

typedef struct {
    bool right_pressed;
    bool connected;
} ResetViewModel;

static void Shake(void) {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
}

void send_serial_command_reset(ESerialCommand command) {
    uint8_t data[1] = {0};

    switch(command) {
    case ESerialCommand_Reset:
        data[0] = MODULE_CONTROL_COMMAND_RESET;
        break;
    default:
        return;
    };

    furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);
}

static void reset_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    ResetViewModel* model = context;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "RESET MODULE");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_draw_str_aligned(canvas, 64, 15, AlignCenter, AlignTop, "Press right to reset");
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignTop, "module or press and hold");
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignTop, "back to return to the menu");

    // Right
    if(model->right_pressed) {
    }
}

static void reset_view_process(ResetView* reset_view, InputEvent* event) {
    with_view_model(
        reset_view->view,
        ResetViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    Shake();
                    send_serial_command_reset(ESerialCommand_Reset);
                } else if(event->key == InputKeyOk) {
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                } else if(event->key == InputKeyOk) {
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                }
            }
        },
        true);
}

static bool reset_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    ResetView* reset_view = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
    } else {
        reset_view_process(reset_view, event);
        consumed = true;
    }

    return consumed;
}

ResetView* reset_view_alloc() {
    ResetView* reset_view = malloc(sizeof(ResetView));
    reset_view->view = view_alloc();
    view_set_context(reset_view->view, reset_view);
    view_allocate_model(reset_view->view, ViewModelTypeLocking, sizeof(ResetViewModel));
    view_set_draw_callback(reset_view->view, reset_view_draw_callback);
    view_set_input_callback(reset_view->view, reset_view_input_callback);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, FLIPPERZERO_SERIAL_BAUD);

    return reset_view;
}

void reset_view_free(ResetView* reset_view) {
    furi_assert(reset_view);
    view_free(reset_view->view);
    free(reset_view);
}

View* reset_view_get_view(ResetView* reset_view) {
    furi_assert(reset_view);
    return reset_view->view;
}

void reset_view_set_data(ResetView* reset_view, bool connected) {
    furi_assert(reset_view);
    with_view_model(
        reset_view->view, ResetViewModel * model, { model->connected = connected; }, true);
}