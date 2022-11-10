#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>

#include "../include/protocol.h"
#include "../include/constants.h"
#include "../include/message_queue.h"

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
} PwnZeroApp;

typedef struct {
    FuriString* text;
} ListElement;

struct PwnDumpModel {
    MessageQueue *queue;

    bool* screen;
    bool* workspace;
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

static bool pwn_zero_exec_cmd(PwnDumpModel* model) {
    if (message_queue_has_message(model->queue)) {
        PwnCommand cmd;
        message_queue_pop_message(model->queue, &cmd);
        FURI_LOG_D("PWN", "Has message (code: %d), processing...", cmd.code);

        // See what the cmd wants
        // Draw a single pixel
        if (cmd.code == 0x01 || cmd.code == 0x00) {
            FURI_LOG_D("PWN", "Drawing pixel at (i: %d, j: %d)", cmd.i, cmd.j);
            if ((cmd.i < FLIPPER_SCREEN_HEIGHT) && (cmd.j < FLIPPER_SCREEN_WIDTH)) {
                model->workspace[cmd.i * FLIPPER_SCREEN_WIDTH + cmd.j] = cmd.code;
            }
        }
        // Flush buffer to the screen
        else if (cmd.code == 0x0f) {
            FURI_LOG_D("PWN", "Flipping buffer...");

            bool* tmp = model->workspace;
            model->workspace = model->screen;
            model->screen = tmp;

            // Copy new buffer to the workspace
            memcpy(model->workspace, model->screen, sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);

            return true;
        }
        // Wipe the buffer
        else if (cmd.code == 0xff) {
            FURI_LOG_D("PWN", "Wiping the buffer...");
            memset(model->workspace, 0, sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);
        }
        else {
            FURI_LOG_D("PWN", "Received an unrecognized command");
        }

    }

    return false;
}

static void pwn_zero_view_draw_callback(Canvas* canvas, void* _model) {
    PwnDumpModel* model = _model;

    for (size_t ii = 0; ii < FLIPPER_SCREEN_HEIGHT; ii++) {
        for (size_t jj = 0; jj < FLIPPER_SCREEN_WIDTH; jj++) {
            if (model->screen[ii * FLIPPER_SCREEN_WIDTH + jj]) {
                canvas_draw_dot(canvas, jj, ii);
            }
        }
    }

}

static bool pwn_zero_view_input_callback(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

static uint32_t pwn_zero_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void pwn_zero_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    furi_assert(context);
    PwnZeroApp* app = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}

static void pwn_zero_push_to_list(PwnDumpModel* model, const char data) {
    message_queue_push_byte(model->queue, data);
}

static int32_t pwn_zero_worker(void* context) {
    furi_assert(context);
    PwnZeroApp* app = context;

    while(1) {
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
                    furi_hal_uart_tx(FuriHalUartIdUSART1, data, length);
                    with_view_model(
                        app->view,
                        PwnDumpModel * model,
                        {
                            for(size_t i = 0; i < length; i++) {
                                pwn_zero_push_to_list(model, data[i]);
                            }
                            update = pwn_zero_exec_cmd(model);
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

static PwnZeroApp* pwn_zero_app_alloc() {
    PwnZeroApp* app = malloc(sizeof(PwnZeroApp));

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
    view_set_draw_callback(app->view, pwn_zero_view_draw_callback);
    view_set_input_callback(app->view, pwn_zero_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(PwnDumpModel));
    with_view_model(
        app->view,
        PwnDumpModel * model,
        {
            model->queue = message_queue_alloc();
            model->screen = malloc(sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);
            model->workspace = malloc(sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);
            memset(model->screen, 0, sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);
            memset(model->workspace, 0, sizeof(bool) * FLIPPER_SCREEN_HEIGHT * FLIPPER_SCREEN_WIDTH);
        },
        true);

    view_set_previous_callback(app->view, pwn_zero_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    // Enable uart listener
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, pwn_zero_on_irq_cb, app);

    app->worker_thread = furi_thread_alloc();
    furi_thread_set_name(app->worker_thread, "UsbUartWorker");
    furi_thread_set_stack_size(app->worker_thread, 1024);
    furi_thread_set_context(app->worker_thread, app);
    furi_thread_set_callback(app->worker_thread, pwn_zero_worker);
    furi_thread_start(app->worker_thread);

    return app;
}

static void pwn_zero_app_free(PwnZeroApp* app) {
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
            free(model->screen);
            free(model->workspace);
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

int32_t pwn_zero_app(void* p) {
    UNUSED(p);
    PwnZeroApp* app = pwn_zero_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    pwn_zero_app_free(app);
    return 0;
}