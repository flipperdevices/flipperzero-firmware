#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>

#define TAG "WIFI_MAP"
#define FILE_NAME "wifi_map_data.csv"

#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>
#include <furi_hal_uart.h>
#include <furi_hal_console.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/dialog_ex.h>
#include <locale/locale.h>

#define MAX_AP_LIST 20
#define WORKER_EVENTS_MASK (WorkerEventStop | WorkerEventRx)

// Screen is 128x64 px
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    View* view;
    FuriThread* worker_thread;
    FuriStreamBuffer* rx_stream;
    File* file;
} WiFiMapApp;

typedef struct WifiMapModel WifiMapModel;

typedef struct {
    FuriString* line;
} StringList;

struct WifiMapModel {
    StringList* lines[MAX_AP_LIST];
    int lnrdy;
    int rdy;
    int cntr;
};

typedef enum {
    WorkerEventReserved = (1 << 0),
    WorkerEventStop = (1 << 1),
    WorkerEventRx = (1 << 2),
} WorkerEventFlags;

const NotificationSequence sequence_notification = {
    &message_display_backlight_on,
    &message_green_255,
    &message_delay_10,
    NULL,
};

File* open_file() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(!storage_file_open(file, APP_DATA_PATH(FILE_NAME), FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_E(TAG, "Failed to open file");
    }
    return file;
}

int32_t write_to_file(char data_line, File* file) {
    char* data = (char*)malloc(sizeof(char) + 1);
    data[0] = data_line;
    if(!storage_file_write(file, data, (uint16_t)strlen(data))) {
        FURI_LOG_E(TAG, "Failed to write to file");
    }
    free(data);
    return 0;
}

int32_t close_file(File* file) {
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return 0;
}

static void retrieve_ap_ssid_distance(const char* data, char* apssid, char* dst) {
    for(size_t i = 0; i < 2; i++) {
        apssid[i] = data[i];
    }

    int cntr = 0;
    for(size_t i = 9; i < strlen(data); i++) {
        if(data[i] == '.')
            break;
        else
            dst[cntr] = data[i];
        cntr++;
    }
}

static void uart_echo_view_draw_callback(Canvas* canvas, void* _model) {
    WifiMapModel* model = _model;
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    int cntr = 0;
    for(size_t i = 0; i < MAX_AP_LIST; i++) {
        const char* line = furi_string_get_cstr(model->lines[i]->line);
        char apssid[2], dst[6];
        retrieve_ap_ssid_distance(line, apssid, dst);
        if(strlen(line) > 0) {
            int d = atoi(dst);
            canvas_draw_circle(canvas, 0, SCREEN_HEIGHT / 2, d);
            if(i % 2) {
                canvas_draw_str(canvas, d, (SCREEN_HEIGHT / 2) + cntr, apssid);
            } else {
                canvas_draw_str(canvas, d, (SCREEN_HEIGHT / 2) - cntr, apssid);
                cntr = cntr + 8;
            }
            if(i > 8) cntr = 8;
        }
    }

    if(model->rdy) {
        canvas_clear(canvas);
        for(size_t i = 0; i < MAX_AP_LIST; i++) {
            furi_string_reset(model->lines[i]->line);
        }
        model->rdy = 0;
        model->cntr = 0;
    }
}

static bool uart_echo_view_input_callback(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

static uint32_t uart_echo_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void uart_echo_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    furi_assert(context);
    WiFiMapApp* app = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}

static void uart_push_to_list(WifiMapModel* model, const char data, WiFiMapApp* app) {
    write_to_file((char)data, app->file);
    if(!model->rdy) {
        if(data != '\n')
            furi_string_push_back(model->lines[model->cntr]->line, data);
        else
            model->cntr++;

        if(model->cntr == MAX_AP_LIST) model->rdy = 1;
    }
}

static int32_t wifi_map_worker(void* context) {
    furi_assert(context);
    WiFiMapApp* app = context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEventStop) break;
        if(events & WorkerEventRx) {
            size_t length = 0;
            do {
                uint8_t data[64];
                length = furi_stream_buffer_receive(app->rx_stream, data, 64, 0);
                if(length > 0) {
                    with_view_model(
                        app->view,
                        WifiMapModel * model,
                        {
                            for(size_t i = 0; i < length; i++) {
                                uart_push_to_list(model, data[i], app);
                            }
                        },
                        false);
                }
            } while(length > 0);

            notification_message(app->notification, &sequence_notification);
            with_view_model(
                app->view, WifiMapModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static WiFiMapApp* wifi_map_app_alloc() {
    WiFiMapApp* app = malloc(sizeof(WiFiMapApp));
    app->file = open_file();
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
    view_set_draw_callback(app->view, uart_echo_view_draw_callback);
    view_set_input_callback(app->view, uart_echo_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(WifiMapModel));

    with_view_model(
        app->view,
        WifiMapModel * model,
        {
            for(size_t i = 0; i < MAX_AP_LIST; i++) {
                model->lines[i] = malloc(sizeof(StringList));
                model->lines[i]->line = furi_string_alloc();
            }
            model->lnrdy = 0;
            model->rdy = 0;
            model->cntr = 0;
        },
        true);

    view_set_previous_callback(app->view, uart_echo_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    app->worker_thread = furi_thread_alloc_ex("UsbUartWorker", 1024, wifi_map_worker, app);
    furi_thread_start(app->worker_thread);

    // Enable uart listener
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, uart_echo_on_irq_cb, app);

    return app;
}

static void wifi_map_app_free(WiFiMapApp* app) {
    furi_assert(app);

    furi_hal_console_enable();

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);

    with_view_model(
        app->view,
        WifiMapModel * model,
        {
            for(size_t i = 0; i < MAX_AP_LIST; i++) {
                free(model->lines[i]);
            }
            free(model);
        },
        true);
    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;

    furi_stream_buffer_free(app->rx_stream);

    close_file(app->file);

    // Free rest
    free(app);
}

int32_t wifi_map_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "wifi_map_app starting...");
    WiFiMapApp* app = wifi_map_app_alloc();
    FuriHalRtcDateTime* rtc = malloc(sizeof(FuriHalRtcDateTime));
    furi_hal_rtc_get_datetime(rtc);
    FuriString* datetime = furi_string_alloc();
    furi_string_printf(
        datetime,
        "##### %d-%d-%d_%d:%d:%d #####\n",
        rtc->day,
        rtc->month,
        rtc->year,
        rtc->hour,
        rtc->minute,
        rtc->second);
    if(!storage_file_write(app->file, furi_string_get_cstr(datetime), furi_string_size(datetime))) {
        FURI_LOG_E(TAG, "Failed to write to file");
    }
    view_dispatcher_run(app->view_dispatcher);
    furi_string_free(datetime);
    free(rtc);
    wifi_map_app_free(app);
    return 0;
}

// Screen is 128x64 px
// x -->
// y
// |
// |
//
