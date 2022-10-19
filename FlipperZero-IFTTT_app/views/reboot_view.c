#include "reboot_view.h"
#include <furi.h>
#include <gui/elements.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <furi_hal_uart.h>

#define MODULE_CONTROL_COMMAND_REBOOT 'r'
#define FLIPPERZERO_SERIAL_BAUD 115200

typedef enum ESerialCommand { ESerialCommand_Reboot } ESerialCommand;

struct RebootView {
    View* view;
};

typedef struct {
    bool right_pressed;
    bool connected;
} RebootViewModel;

static void Shake(void) {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
}

void send_serial_command_reboot(ESerialCommand command) {
    uint8_t data[1] = {0};

    switch(command) {
    case ESerialCommand_Reboot:
        data[0] = MODULE_CONTROL_COMMAND_REBOOT;
        break;
    default:
        return;
    };

    furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);
}

static void reboot_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    RebootViewModel* model = context;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "REBOOT MODULE");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_draw_str_aligned(canvas, 64, 15, AlignCenter, AlignTop, "Press right to restart the");
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignTop, "module or press and hold");
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignTop, "back to return to the menu");

    // Right
    if(model->right_pressed) {
    }
}

static void reboot_view_process(RebootView* reboot_view, InputEvent* event) {
    with_view_model(
        reboot_view->view,
        RebootViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    Shake();
                    send_serial_command_reboot(ESerialCommand_Reboot);
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

static bool reboot_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    RebootView* reboot_view = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
    } else {
        reboot_view_process(reboot_view, event);
        consumed = true;
    }

    return consumed;
}

RebootView* reboot_view_alloc() {
    RebootView* reboot_view = malloc(sizeof(RebootView));
    reboot_view->view = view_alloc();
    view_set_context(reboot_view->view, reboot_view);
    view_allocate_model(reboot_view->view, ViewModelTypeLocking, sizeof(RebootViewModel));
    view_set_draw_callback(reboot_view->view, reboot_view_draw_callback);
    view_set_input_callback(reboot_view->view, reboot_view_input_callback);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, FLIPPERZERO_SERIAL_BAUD);

    return reboot_view;
}

void reboot_view_free(RebootView* reboot_view) {
    furi_assert(reboot_view);
    view_free(reboot_view->view);
    free(reboot_view);
}

View* reboot_view_get_view(RebootView* reboot_view) {
    furi_assert(reboot_view);
    return reboot_view->view;
}

void reboot_view_set_data(RebootView* reboot_view, bool connected) {
    furi_assert(reboot_view);
    with_view_model(
        reboot_view->view, RebootViewModel * model, { model->connected = connected; }, true);
}