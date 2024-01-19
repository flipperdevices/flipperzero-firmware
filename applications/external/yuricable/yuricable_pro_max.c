#include <yuricable_pro_max.h>
#include <yuricable_pro_max_icons.h>

#define TAG "YURICABLE_PRO_MAX"
#define SDQ_PIN gpio_ext_pa7 // GPIO 2

static void yuricable_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void yuricable_render_callback(Canvas* canvas, void* ctx) {
    YuriCableContext* yuricable_context = ctx;
    if(furi_mutex_acquire(yuricable_context->mutex, 200) != FuriStatusOk) {
        return;
    }
    YuriCableData* data = yuricable_context->data;
    UNUSED(data);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 13, "YuriCable Pro Max");

    canvas_draw_icon_animation(canvas, 115, 4, yuricable_context->data->listeningAnimation);

    canvas_draw_line(canvas, 0, 16, 127, 16);
    canvas_draw_line(canvas, 0, 28, 127, 28);
    canvas_draw_line(canvas, 0, 40, 127, 40);
    canvas_draw_line(canvas, 0, 52, 127, 52);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 26, "DCSD - Bootlog");
    //canvas_draw_str(canvas, 4, 38, "JTAG / SWD");
    canvas_draw_str(canvas, 4, 38, "Reset");
    canvas_draw_str(canvas, 4, 50, "DFU");
    //canvas_draw_str(canvas, 4, 61, "Recovery");

    canvas_draw_icon(
        canvas, 120, 7 + 12 * yuricable_context->data->sdq->runCommand, &I_ButtonLeft_4x7);

    furi_mutex_release(yuricable_context->mutex);
}

FuriString* yuricable_command_callback(char* command, void* ctx) {
    YuriCableContext* yuricable_context = ctx;
    UNUSED(yuricable_context);
    if(strcmp(command, "start") == 0) {
        if(yuricable_context->data->sdq->listening) {
            return furi_string_alloc_printf("already listening");
        }
        sdq_device_start(yuricable_context->data->sdq);
        icon_animation_start(yuricable_context->data->listeningAnimation);
        return furi_string_alloc_printf("started");
    }
    if(strcmp(command, "stop") == 0) {
        if(!yuricable_context->data->sdq->listening) {
            return furi_string_alloc_printf("already stopped");
        }
        sdq_device_stop(yuricable_context->data->sdq);
        icon_animation_stop(yuricable_context->data->listeningAnimation);
        return furi_string_alloc_printf("stopped");
    }
    if(strncmp(command, "mode", 4) == 0) {
        if(command[4] == ' ') {
            char* mode = command + 5;
            if(strcmp(mode, "dfu") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_DFU;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode dfu");
            }
            if(strcmp(mode, "reset") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_RESET;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode reset");
            }
            if(strcmp(mode, "dcsd") == 0) {
                yuricable_context->data->sdq->runCommand = SDQDeviceCommand_DCSD;
                yuricable_context->data->sdq->commandExecuted = false;
                return furi_string_alloc_printf("set mode dcsd");
            }
        }
        return furi_string_alloc_printf("use: /mode <dfu | reset | dcsd>");
    }
    if(strncmp(command, "help", 4) == 0) {
        return furi_string_alloc_printf(
            "commands:\r\n/start\r\n/stop\r\n/mode <dfu | reset | dcsd>");
    }
    return furi_string_alloc_printf("%s is no valid command", command);
}

int32_t yuricable_pro_max_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Starting the SDQ Listener on GPIO 2!");
    // Configure our initial data.
    YuriCableContext* yuricable_context = malloc(sizeof(YuriCableContext));
    yuricable_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    yuricable_context->data = malloc(sizeof(YuriCableData));
    yuricable_context->data->listeningAnimation = icon_animation_alloc(&A_Round_loader_8x8);

    // Queue for events (tick or input)
    yuricable_context->queue = furi_message_queue_alloc(8, sizeof(Event));

    UsbUartConfig bridgeConfig = {
        .vcp_ch = 1, .uart_ch = 0, .baudrate_mode = 0, .baudrate = 115200};
    UsbUartBridge* uartBridge = usb_uart_enable(&bridgeConfig);
    usb_uart_set_command_callback(uartBridge, yuricable_command_callback, yuricable_context);

    yuricable_context->data->sdq = sdq_device_alloc(&SDQ_PIN, uartBridge);
    yuricable_context->data->sdq->runCommand = SDQDeviceCommand_DCSD;

    //  Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, yuricable_render_callback, yuricable_context);
    view_port_input_callback_set(view_port, yuricable_input_callback, yuricable_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Event event;
    bool processing = true;
    do {
        if(furi_message_queue_get(yuricable_context->queue, &event, 300) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch(event.type) {
            case EventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    FURI_LOG_I(TAG, "Short-Back pressed. Exiting program.");
                    processing = false;
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyOk) {
                    FURI_LOG_I(TAG, "Pressed Enter Key");
                    if(!yuricable_context->data->sdq->listening) {
                        sdq_device_start(yuricable_context->data->sdq);
                        icon_animation_start(yuricable_context->data->listeningAnimation);
                    } else {
                        sdq_device_stop(yuricable_context->data->sdq);
                        icon_animation_stop(yuricable_context->data->listeningAnimation);
                    }
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyUp) {
                    if(yuricable_context->data->sdq->runCommand > 1) {
                        yuricable_context->data->sdq->runCommand--;
                        yuricable_context->data->sdq->commandExecuted = false;
                    }
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyDown) {
                    if(yuricable_context->data->sdq->runCommand < 3) {
                        yuricable_context->data->sdq->runCommand++;
                        yuricable_context->data->sdq->commandExecuted = false;
                    }
                }
                break;
            }
        }

        view_port_update(view_port);
    } while(processing);

    usb_uart_disable(uartBridge);
    free(uartBridge);

    sdq_device_free(yuricable_context->data->sdq);
    icon_animation_free(yuricable_context->data->listeningAnimation);
    free(yuricable_context->data);
    free(yuricable_context);

    furi_hal_gpio_init(&SDQ_PIN, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&SDQ_PIN, true);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
