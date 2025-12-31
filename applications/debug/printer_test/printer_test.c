#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_usb_printer.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <gui/elements.h>
#include <string.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    size_t bytes_received;
    size_t total_bytes;
    bool job_active;
    bool usb_connected;
    char last_data[64];  // Store last chunk of printable data for preview
    FuriTimer* poll_timer;
    PrinterCallbacks* printer_callbacks;
    char current_job_name[64];  // Current job name from PJL
    char line_buffer[256];      // Buffer for parsing lines
    size_t line_pos;           // Current position in line buffer
    bool job_complete;         // Flag for job completion
} PrinterTest;

typedef enum {
    EventTypeKey,
    EventTypeTick,
    EventTypeDataReceived,
    EventTypeStatusChanged,
    EventTypeJobStarted,
    EventTypeJobComplete,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
    size_t data_len;
    bool connected;
    char job_name[64];  // Job name for job events
} PrinterTestEvent;

// Helper function to parse PJL commands
static void parse_pjl_command(PrinterTest* app, const char* line) {
    const char* pjl_start = line;
    
    // Check if line starts with 0x041B%-12345X (UEL sequence) followed by @PJL
    if(line[0] == 0x04 && line[1] == 0x1B && strncmp(line + 2, "%-12345X", 8) == 0) {
        // Skip the UEL sequence and look for @PJL
        pjl_start = line + 10; // Skip 0x041B%-12345X
        // Skip any whitespace after UEL
        while(*pjl_start == ' ' || *pjl_start == '\t' || *pjl_start == '\r' || *pjl_start == '\n') {
            pjl_start++;
        }
    }
    
    // Check if we have @PJL at the current position
    if(strncmp(pjl_start, "@PJL", 4) != 0) {
        return;
    }
    
    // Skip @PJL and any spaces
    const char* cmd = pjl_start + 4;
    while(*cmd == ' ' || *cmd == '\t') cmd++;
    
    // Check for SET JOBNAME or JOBNAME command
    if(strncmp(cmd, "SET JOBNAME", 11) == 0) {
        const char* jobname = cmd + 11;
        while(*jobname == ' ' || *jobname == '\t' || *jobname == '=') jobname++;
        
        // Extract job name (remove quotes if present)
        if(*jobname == '"') {
            jobname++;
            const char* end = strchr(jobname, '"');
            if(end) {
                size_t len = end - jobname;
                if(len < sizeof(app->current_job_name)) {
                    strncpy(app->current_job_name, jobname, len);
                    app->current_job_name[len] = '\0';
                }
            }
        } else {
            // No quotes, copy until space or end
            size_t i = 0;
            while(jobname[i] && jobname[i] != ' ' && jobname[i] != '\t' && 
                  jobname[i] != '\r' && jobname[i] != '\n' && 
                  i < sizeof(app->current_job_name) - 1) {
                app->current_job_name[i] = jobname[i];
                i++;
            }
            app->current_job_name[i] = '\0';
        }
        
        // Send job started event
        PrinterTestEvent event = {
            .type = EventTypeJobStarted,
        };
        strncpy(event.job_name, app->current_job_name, sizeof(event.job_name) - 1);
        furi_message_queue_put(app->event_queue, &event, 0);
        
    } else if(strncmp(cmd, "JOBNAME", 7) == 0) {
        const char* jobname = cmd + 7;
        while(*jobname == ' ' || *jobname == '\t' || *jobname == '=') jobname++;
        
        // Extract job name (same logic as above)
        if(*jobname == '"') {
            jobname++;
            const char* end = strchr(jobname, '"');
            if(end) {
                size_t len = end - jobname;
                if(len < sizeof(app->current_job_name)) {
                    strncpy(app->current_job_name, jobname, len);
                    app->current_job_name[len] = '\0';
                }
            }
        } else {
            size_t i = 0;
            while(jobname[i] && jobname[i] != ' ' && jobname[i] != '\t' && 
                  jobname[i] != '\r' && jobname[i] != '\n' && 
                  i < sizeof(app->current_job_name) - 1) {
                app->current_job_name[i] = jobname[i];
                i++;
            }
            app->current_job_name[i] = '\0';
        }
        
        // Send job started event
        PrinterTestEvent event = {
            .type = EventTypeJobStarted,
        };
        strncpy(event.job_name, app->current_job_name, sizeof(event.job_name) - 1);
        furi_message_queue_put(app->event_queue, &event, 0);
        
    } else if(strncmp(cmd, "EOJ", 3) == 0) {
        // End of job
        PrinterTestEvent event = {
            .type = EventTypeJobComplete,
        };
        furi_message_queue_put(app->event_queue, &event, 0);
    }
}

// Helper function to process received data line by line
static void process_data_lines(PrinterTest* app, const uint8_t* data, size_t len) {
    for(size_t i = 0; i < len; i++) {
        if(data[i] == 0x0A) {  // Line feed (newline)
            // Null-terminate the line
            app->line_buffer[app->line_pos] = '\0';
            
            // Parse the line for PJL commands
            parse_pjl_command(app, app->line_buffer);
            
            // Reset line buffer
            app->line_pos = 0;
        } else if(data[i] != 0x0D) {  // Ignore carriage return
            // Add character to line buffer if there's space
            if(app->line_pos < sizeof(app->line_buffer) - 1) {
                app->line_buffer[app->line_pos++] = data[i];
            }
        }
    }
}

// Callback for receiving print data
static void printer_data_callback(const uint8_t* data, size_t len, void* context) {
    PrinterTest* app = context;
    
    // Process data line by line for PJL command parsing
    process_data_lines(app, data, len);
    
    PrinterTestEvent event = {
        .type = EventTypeDataReceived,
        .data_len = len,
    };
    furi_message_queue_put(app->event_queue, &event, 0);
}

// Callback for USB connection status
static void printer_status_callback(bool connected, void* context) {
    PrinterTest* app = context;
    
    PrinterTestEvent event = {
        .type = EventTypeStatusChanged,
        .connected = connected,
    };
    furi_message_queue_put(app->event_queue, &event, 0);
}

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
    if(app->usb_connected) {
        canvas_draw_str(canvas, 2, 24, "Status: Connected");
    } else {
        canvas_draw_str(canvas, 2, 24, "Status: Disconnected");
    }

    // Draw job status
    if(app->job_complete) {
        canvas_draw_str(canvas, 2, 36, "Printing complete!");
        if(app->current_job_name[0] != '\0') {
            char buf[80];
            snprintf(buf, sizeof(buf), "Job: %s", app->current_job_name);
            canvas_draw_str(canvas, 2, 48, buf);
        }
        char buf[32];
        snprintf(buf, sizeof(buf), "Total: %zu bytes", app->total_bytes);
        canvas_draw_str(canvas, 2, 60, buf);
    } else if(app->job_active) {
        if(app->current_job_name[0] != '\0') {
            char buf[80];
            snprintf(buf, sizeof(buf), "Now printing: %s", app->current_job_name);
            canvas_draw_str(canvas, 2, 36, buf);
        } else {
            canvas_draw_str(canvas, 2, 36, "Print job active");
        }
        
        char buf[32];
        snprintf(buf, sizeof(buf), "Total: %zu bytes", app->total_bytes);
        canvas_draw_str(canvas, 2, 48, buf);
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
    app->usb_connected = false;
    app->last_data[0] = '\0';
    app->current_job_name[0] = '\0';
    app->line_pos = 0;
    app->job_complete = false;

    // Allocate printer callbacks
    app->printer_callbacks = malloc(sizeof(PrinterCallbacks));
    app->printer_callbacks->data_rx_callback = printer_data_callback;
    app->printer_callbacks->status_callback = printer_status_callback;

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

    // Set up structured printer callbacks
    furi_hal_usb_printer_set_callbacks(app->printer_callbacks, app);

    PrinterTestEvent event;
    bool running = true;

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
                        app->current_job_name[0] = '\0';
                        app->job_complete = false;
                        app->line_pos = 0;
                        break;
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTypeDataReceived) {
                // Handle incoming print data via callback
                app->job_active = true;
                app->total_bytes += event.data_len;
                view_port_update(app->view_port);
            } else if(event.type == EventTypeStatusChanged) {
                // Handle USB connection status change
                app->usb_connected = event.connected;
                view_port_update(app->view_port);
            } else if(event.type == EventTypeJobStarted) {
                // Handle job started event
                app->job_active = true;
                app->job_complete = false;
                strncpy(app->current_job_name, event.job_name, sizeof(app->current_job_name) - 1);
                app->current_job_name[sizeof(app->current_job_name) - 1] = '\0';
                view_port_update(app->view_port);
            } else if(event.type == EventTypeJobComplete) {
                // Handle job completion event
                app->job_complete = true;
                view_port_update(app->view_port);
            }
        }
    }

    // Cleanup
    furi_hal_usb_printer_set_callbacks(NULL, NULL);
    furi_timer_stop(app->poll_timer);
    furi_timer_free(app->poll_timer);
    furi_hal_usb_set_config(usb_mode_prev, NULL);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    free(app->printer_callbacks);
    free(app);

    return 0;
}
