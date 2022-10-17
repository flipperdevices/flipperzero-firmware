#include "module_view.h"
#include <furi.h>
#include <gui/elements.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <furi_hal_uart.h>

#define MODULE_CONTROL_COMMAND_CONFIG_ON 'c'
#define MODULE_CONTROL_COMMAND_CONFIG_OFF 'd'
#define FLIPPERZERO_SERIAL_BAUD 115200

bool configState;

typedef enum ESerialCommand { ESerialCommand_Config_On, ESerialCommand_Config_Off } ESerialCommand;

struct ModuleView {
    View* view;
};

typedef struct {
    bool right_pressed;
    bool connected;
} ModuleViewModel;

/*
static void Shake(void) {
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_single_vibro);
    furi_record_close(RECORD_NOTIFICATION);
}
*/

void send_serial_command_module(ESerialCommand command) {
    uint8_t data[1] = {0};

    switch(command) {
    case ESerialCommand_Config_On:
        data[0] = MODULE_CONTROL_COMMAND_CONFIG_ON;
        break;
    case ESerialCommand_Config_Off:
        data[0] = MODULE_CONTROL_COMMAND_CONFIG_OFF;
        break;
    default:
        return;
    };

    furi_hal_uart_tx(FuriHalUartIdUSART1, data, 1);
}

static void module_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    ModuleViewModel* model = context;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, "CONFIGURE MODULE");
    canvas_draw_line(canvas, 0, 10, 128, 10);
    canvas_draw_str_aligned(canvas, 64, 15, AlignCenter, AlignTop, "Press right to start the");
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignTop, "configurator and connect");
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignTop, "to ip 192.168.4.1 or press");
    canvas_draw_str_aligned(canvas, 64, 45, AlignCenter, AlignTop, "and hold back to return");
    canvas_draw_str_aligned(canvas, 64, 55, AlignCenter, AlignTop, "to the menu");

    if(configState == false) {
        send_serial_command_module(ESerialCommand_Config_On);
        configState = true;
    }

    // Right
    if(model->right_pressed) {
    }
}

static void module_view_process(ModuleView* module_view, InputEvent* event) {
    with_view_model(
        module_view->view,
        ModuleViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
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

static bool module_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    ModuleView* module_view = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        send_serial_command_module(ESerialCommand_Config_Off);
        configState = false;
    } else {
        module_view_process(module_view, event);
        consumed = true;
    }

    return consumed;
}

ModuleView* module_view_alloc() {
    ModuleView* module_view = malloc(sizeof(ModuleView));
    module_view->view = view_alloc();
    view_set_context(module_view->view, module_view);
    view_allocate_model(module_view->view, ViewModelTypeLocking, sizeof(ModuleViewModel));
    view_set_draw_callback(module_view->view, module_view_draw_callback);
    view_set_input_callback(module_view->view, module_view_input_callback);
    furi_hal_uart_set_br(FuriHalUartIdUSART1, FLIPPERZERO_SERIAL_BAUD);
    configState = false;
    return module_view;
}

void module_view_free(ModuleView* module_view) {
    furi_assert(module_view);
    view_free(module_view->view);
    free(module_view);
}

View* module_view_get_view(ModuleView* module_view) {
    furi_assert(module_view);
    return module_view->view;
}

void module_view_set_data(ModuleView* module_view, bool connected) {
    furi_assert(module_view);
    with_view_model(
        module_view->view, ModuleViewModel * model, { model->connected = connected; }, true);
}