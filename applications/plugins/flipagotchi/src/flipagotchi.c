#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "../include/pwnagotchi.h"
#include "../include/protocol.h"
#include "../include/constants.h"
#include "../include/message_queue.h"
#include "Flipagotchi_icons.h"

#define LINES_ON_SCREEN 6
#define COLUMNS_ON_SCREEN 21

typedef struct PwnDumpModel PwnDumpModel;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    View* view;
    FuriThread* worker_thread;
    FuriStreamBuffer* rx_stream;
} FlipagotchiApp;

typedef struct {
    FuriString* text;
} ListElement;

struct PwnDumpModel {
    MessageQueue* queue;

    Pwnagotchi* pwn;
};

typedef enum {
    WorkerEventReserved = (1 << 0), // Reserved for StreamBuffer internal event
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

const NotificationSequence sequence_notification = {
    &message_display_backlight_on,
    &message_green_255,
    &message_delay_10,
    NULL,
};

static bool flipagotchi_exec_cmd(PwnDumpModel* model) {
    if(message_queue_has_message(model->queue)) {
        PwnCommand cmd;
        message_queue_pop_message(model->queue, &cmd);
        FURI_LOG_D("PWN", "Has message (code: %d), processing...", cmd.parameterCode);

        // See what the cmd wants
        switch(cmd.parameterCode) {
        // Process Face
        case 0x04: {
            // Adding 4 to account for the offset that is required above 0x03
            int face = cmd.arguments[0] - 4;

            if(face < 0) {
                face = 0;
            }

            model->pwn->face = cmd.arguments[0] - 4;

            break;
        }
        // Process Name
        case 0x05: {
            // Write over hostname with nothing
            strncpy(model->pwn->hostname, "", PWNAGOTCHI_MAX_HOSTNAME_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_HOSTNAME_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }

                model->pwn->hostname[i] = cmd.arguments[i];
            }
            break;
        }
        // Process channel
        case 0x06: {
            // Write over channel with nothing
            strncpy(model->pwn->channel, "", PWNAGOTCHI_MAX_CHANNEL_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_CHANNEL_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }

                model->pwn->channel[i] = cmd.arguments[i];
            }
            break;
        }
        // Process APS (Access Points)
        case 0x07: {
            // Write over APS with nothing
            strncpy(model->pwn->apStat, "", PWNAGOTCHI_MAX_APS_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_APS_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }
                model->pwn->apStat[i] = cmd.arguments[i];
            }
            break;
        }
        // Process uptime
        case 0x08: {
            // Write over uptime with nothing
            strncpy(model->pwn->uptime, "", PWNAGOTCHI_MAX_UPTIME_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_UPTIME_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }
                model->pwn->uptime[i] = cmd.arguments[i];
            }
            break;
        }
        // Process friend
        case 0x09: {
            // Friend not implemented yet
            break;
        }
        // Process mode
        case 0x0a: {
            enum PwnagotchiMode mode;

            switch(cmd.arguments[0]) {
            case 0x04:
                mode = PwnMode_Manual;
                break;
            case 0x05:
                mode = PwnMode_Auto;
                break;
            case 0x06:
                mode = PwnMode_Ai;
                break;
            default:
                mode = PwnMode_Manual;
                break;
            }
            model->pwn->mode = mode;

            break;
        }
        // Process Handshakes
        case 0x0b: {
            // Write over handshakes with nothing
            strncpy(model->pwn->handshakes, "", PWNAGOTCHI_MAX_HANDSHAKES_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_HANDSHAKES_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }
                model->pwn->handshakes[i] = cmd.arguments[i];
            }
            break;
        }
        // Process message
        case 0x0c: {
            // Write over the message with nothing
            strncpy(model->pwn->message, "", PWNAGOTCHI_MAX_MESSAGE_LEN);

            for(size_t i = 0; i < PWNAGOTCHI_MAX_MESSAGE_LEN; i++) {
                // Break if we hit the end of the name
                if(cmd.arguments[i] == 0x00) {
                    break;
                }
                model->pwn->message[i] = cmd.arguments[i];
            }
            break;
        }
        }
    }

    return false;
}

static void flipagotchi_view_draw_callback(Canvas* canvas, void* _model) {
    PwnDumpModel* model = _model;

    pwnagotchi_draw_all(model->pwn, canvas);
}

static bool flipagotchi_view_input_callback(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

static uint32_t flipagotchi_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void flipagotchi_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    furi_assert(context);
    FlipagotchiApp* app = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}

static void flipagotchi_push_to_list(PwnDumpModel* model, const char data) {
    message_queue_push_byte(model->queue, data);
}

static int32_t flipagotchi_worker(void* context) {
    furi_assert(context);
    FlipagotchiApp* app = context;

    while(true) {
        bool update = false;
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEventStop) break;
        if(events & WorkerEventRx) {
            size_t length = 0;
            do {
                uint8_t data[1];
                length = furi_stream_buffer_receive(app->rx_stream, data, 1, 0);
                if(length > 0) {
                    with_view_model(
                        app->view,
                        PwnDumpModel * model,
                        {
                            for(size_t i = 0; i < length; i++) {
                                flipagotchi_push_to_list(model, data[i]);
                            }
                            update = flipagotchi_exec_cmd(model);
                        },
                        update);
                }
            } while(length > 0);

            notification_message(app->notification, &sequence_notification);
            // with_view_model(
            // app->view, PwnDumpModel * model, { UNUSED(model); }, true);
        }
    }
    return 0;
}

static FlipagotchiApp* flipagotchi_app_alloc() {
    FlipagotchiApp* app = malloc(sizeof(FlipagotchiApp));

    app->rx_stream = furi_stream_buffer_alloc(2048, 1);

    // Gui
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_draw_callback(app->view, flipagotchi_view_draw_callback);
    view_set_input_callback(app->view, flipagotchi_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(PwnDumpModel));
    with_view_model(
        app->view,
        PwnDumpModel * model,
        {
            model->queue = message_queue_alloc();
            model->pwn = pwnagotchi_alloc();
        },
        true);

    view_set_previous_callback(app->view, flipagotchi_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    // Enable uart listener
    furi_hal_console_disable();
    furi_hal_uart_set_br(PWNAGOTCHI_UART_CHANNEL, PWNAGOTCHI_UART_BAUD);
    furi_hal_uart_set_irq_cb(PWNAGOTCHI_UART_CHANNEL, flipagotchi_on_irq_cb, app);

    app->worker_thread = furi_thread_alloc();
    furi_thread_set_name(app->worker_thread, "UsbUartWorker");
    furi_thread_set_stack_size(app->worker_thread, 1024);
    furi_thread_set_context(app->worker_thread, app);
    furi_thread_set_callback(app->worker_thread, flipagotchi_worker);
    furi_thread_start(app->worker_thread);

    return app;
}

static void flipagotchi_app_free(FlipagotchiApp* app) {
    furi_assert(app);

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

    furi_hal_console_enable();

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);

    with_view_model(
        app->view,
        PwnDumpModel * model,
        {
            message_queue_free(model->queue);
            pwnagotchi_free(model->pwn);
        },
        true);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;

    furi_stream_buffer_free(app->rx_stream);

    // Free rest
    free(app);
}

int32_t flipagotchi_app(void* p) {
    UNUSED(p);
    FlipagotchiApp* app = flipagotchi_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    flipagotchi_app_free(app);
    return 0;
}