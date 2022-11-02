#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "../include/message_queue.h"
#include "../include/constants.h"
#include "../include/pwnagotchi.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* viewDispatcher;
    ViewPort* viewPort;
    View* view;
    FuriThread* workerThread;
    FuriStreamBuffer* rxStream;
    MessageQueue* messageQueue;
    Pwnagotchi* pwnagotchi;

} PwnZeroApp;

typedef enum {
    WorkerEventReserved = (1 << 0),
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

static void pwn_zero_on_irq_cb(UartIrqEvent event, uint8_t data, void* context) {
    furi_assert(context);
    PwnZeroApp* app = context;

    if (event == UartIrqEventRXNE) {
        furi_stream_buffer_send(app->rxStream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->workerThread), WorkerEventRx);
    }
}

// static void pwn_zero_view_draw_callback(Canvas* canvas, void* model) {

// }

static int32_t pwn_zero_worker(void* context) {
    furi_assert(context);
    PwnZeroApp* app = context;

    while (true) {
        uint32_t events = furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if (events & WorkerEventStop) {
            break;
        }

        if (events & WorkerEventRx) {
            size_t length = 0;

            do {
                uint8_t data;
                length = furi_stream_buffer_receive(app->rxStream, &data, 1, 0);

                // If there was data received
                if (length > 0) {
                    // Add it to the messageQueue
                    message_queue_push_byte(app->messageQueue, data);
                }
            } while (length > 0);

        }

        // Otherwise digest a message if present
        if (message_queue_has_message(app->messageQueue)) {
            PwnCommand cmd;
            if (message_queue_pop_message(app->messageQueue, &cmd)) {
                // Determine what command we are doing
                // Add pixel to buffer
                if (cmd.code == 0x00 || cmd.code == 0x01) {
                    pwnagotchi_screen_set(app->pwnagotchi, cmd.i, cmd.j, cmd.code);
                }
                // Flush buffer to screen
                else if (cmd.code == 0x0f) {
                    view_port_update(app->viewPort);
                }
                // Clear buffer
                else if (cmd.code == 0xff) {
                    pwnagotchi_screen_clear(app->pwnagotchi);
                }
                else {
                    message_queue_validate(app->messageQueue);
                }

            }
            else {
                message_queue_validate(app->messageQueue);
            }
        }

    }

    return 0;
}

static PwnZeroApp* pwn_zero_app_alloc() {
    PwnZeroApp* app = malloc(sizeof(PwnZeroApp));

    app->pwnagotchi = pwnagotchi_alloc();

    // Setup stream for receive
    app->rxStream = furi_stream_buffer_alloc(2048, 1);

    // Setup view dispatcher
    app->viewDispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->viewDispatcher);
    view_dispatcher_attach_to_gui(app->viewDispatcher, app->gui, ViewDispatcherTypeFullscreen);


    app->viewPort = view_port_alloc();
    view_port_draw_callback_set(app->viewPort, pwnagotchi_screen_flush, app->pwnagotchi);

    // view_set_draw_callback(app->view, pwnagotchi_screen_flush, app->pwnagotchi);

    // View
    app->view = view_alloc();
    // TODO: draw callback
    // TODO: input callback

    // Uart listener
    furi_hal_console_disable();
    furi_hal_uart_set_br(PWNAGOTCHI_UART_CHANNEL, PWNAGOTCHI_UART_BAUD);
    furi_hal_uart_set_irq_cb(PWNAGOTCHI_UART_CHANNEL, pwn_zero_on_irq_cb, app);

    // Setup the worker thread
    app->workerThread = furi_thread_alloc();
    furi_thread_set_name(app->workerThread, "UartWorker");
    furi_thread_set_stack_size(app->workerThread, 1024);
    furi_thread_set_context(app->workerThread, app);
    furi_thread_set_callback(app->workerThread, pwn_zero_worker);
    furi_thread_start(app->workerThread);

    return app;
}

static void pwn_zero_app_free(PwnZeroApp* app) {
    furi_assert(app);

    furi_thread_flags_set(furi_thread_get_id(app->workerThread), WorkerEventStop);
    furi_thread_join(app->workerThread);
    furi_thread_free(app->workerThread);

    furi_hal_console_enable();

    view_dispatcher_remove_view(app->viewDispatcher, 0);

    view_free(app->view);
    view_dispatcher_free(app->viewDispatcher);

    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    furi_stream_buffer_free(app->rxStream);

    pwnagotchi_free(app->pwnagotchi);

    free(app);

    app = NULL;
}

int32_t pwn_zero_app(void* p) {
    UNUSED(p);
    PwnZeroApp* app = pwn_zero_app_alloc();
    view_dispatcher_run(app->viewDispatcher);
    pwn_zero_app_free(app);

    return 0;
}