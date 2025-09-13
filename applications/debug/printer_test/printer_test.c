#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_printer.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <gui/elements.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    size_t bytes_received;
    size_t total_bytes;
    bool job_active;
    char last_data[64];  // Store last chunk of printable data for preview
    FuriTimer* poll_timer;
} PrinterTest;

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PrinterTestEvent;

// Timer callback to poll for received data
static void printer_test_timer_callback(void* context) {
    PrinterTest* app = context;
    
    PrinterTestEvent event = {
        .type = EventTypeTick,
    };
    furi_message_queue_put(app->event_queue, &event, 0);
}

static void printer_test_draw_callback(Canvas* canvas, void* context) {
    PrinterTest* app = context;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "USB Printer Monitor");

    canvas_set_font(canvas, FontSecondary);

    // Draw connection status
    if(furi_hal_usb_printer_is_connected()) {
        canvas_draw_str(canvas, 2, 24, "Status: Connected");
    } else {
        canvas_draw_str(canvas, 2, 24, "Status: Disconnected");
    }

    // Draw job status
    if(app->job_active) {
        canvas_draw_str(canvas, 2, 36, "Print job active");
        char buf[32];
        snprintf(buf, sizeof(buf), "Total: %zu bytes", app->total_bytes);
        canvas_draw_str(canvas, 2, 48, buf);
        
        // Draw preview if available
        if(app->last_data[0] != '\0') {
            canvas_draw_str(canvas, 2, 60, app->last_data);
        }
    } else {
        canvas_draw_str(canvas, 2, 36, "Waiting for print job...");
        canvas_draw_str(canvas, 2, 48, "Send data from computer");
    }
}

static void printer_test_input_callback(InputEvent* input_event, void* context) {
    PrinterTest* app = context;

    PrinterTestEvent event = {
        .type = EventTypeKey,
        .input = *input_event
    };
    furi_message_queue_put(app->event_queue, &event, FuriWaitForever);
}

int32_t printer_test_app(void* p) {
    UNUSED(p);

    PrinterTest* app = malloc(sizeof(PrinterTest));
    app->event_queue = furi_message_queue_alloc(8, sizeof(PrinterTestEvent));
    app->bytes_received = 0;
    app->total_bytes = 0;
    app->job_active = false;
    app->last_data[0] = '\0';

    // Create GUI
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, printer_test_draw_callback, app);
    view_port_input_callback_set(app->view_port, printer_test_input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    // Create timer for polling received data
    app->poll_timer = furi_timer_alloc(printer_test_timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->poll_timer, 100); // Poll every 100ms

    // Save previous USB mode
    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    
    // Switch to printer mode
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_printer, NULL) == true);

    PrinterTestEvent event;
    bool running = true;
    uint8_t buffer[256];

    while(running) {
        FuriStatus event_status = furi_message_queue_get(app->event_queue, &event, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeKey) {
                // Handle key events
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyBack:
                        running = false;
                        break;
                    case InputKeyOk:
                        // Reset counters
                        app->job_active = false;
                        app->total_bytes = 0;
                        app->last_data[0] = '\0';
                        break;
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                // Poll for received data
                int32_t len = furi_hal_usb_printer_receive(buffer, sizeof(buffer));
                if(len > 0) {
                    app->job_active = true;
                    app->total_bytes += len;
                    
                    // Extract printable characters for preview
                    size_t preview_len = 0;
                    for(int32_t i = 0; i < len && preview_len < sizeof(app->last_data) - 1; i++) {
                        if(buffer[i] >= 32 && buffer[i] <= 126) {
                            app->last_data[preview_len++] = buffer[i];
                        } else if(buffer[i] == '\n' && preview_len < sizeof(app->last_data) - 3) {
                            app->last_data[preview_len++] = '\\';
                            app->last_data[preview_len++] = 'n';
                        }
                    }
                    app->last_data[preview_len] = '\0';
                    
                    view_port_update(app->view_port);
                }
            }
        }
    }

    // Cleanup
    furi_timer_stop(app->poll_timer);
    furi_timer_free(app->poll_timer);
    furi_hal_usb_set_config(usb_mode_prev, NULL);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app);

    return 0;
}
