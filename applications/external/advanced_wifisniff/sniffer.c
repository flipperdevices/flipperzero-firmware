#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>
#include <toolbox/stream/file_stream.h>
#include <cfw.h>

#include "helpers/minmea.h"
#include "wifisniffer_icons.h"

#define appname "ll-wifisniffer"

#define RX_BUF_SIZE 2048
#define MAX_ACCESS_POINTS 2048 // imagine getting this many access points

#define MAX_SSID_LENGTH 32
#define MAX_BSSID_LENGTH 18

#define UART_CH_ESP \
    (CFW_SETTINGS()->uart_esp_channel == UARTDefault ? FuriHalUartIdUSART1 : FuriHalUartIdLPUART1)

#define UART_CH_GPS \
    (CFW_SETTINGS()->uart_nmea_channel == UARTDefault ? FuriHalUartIdUSART1 : FuriHalUartIdLPUART1)

#define WORKER_ALL_RX_EVENTS (WorkerEvtStop | WorkerEvtRxDone)

typedef enum {
    WorkerEvtStop = (1 << 0),
    WorkerEvtRxDone = (1 << 1),
} WorkerEvtFlags;

typedef enum {
    EventTypeKey,
    EventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

typedef struct {
    char* recievedMac;
    char* sentMac;
} Packet;

typedef struct {
    char* ssid;
    char* bssid;
    int8_t rssi;
    uint8_t channel;
    FuriHalRtcDateTime datetime;
    uint16_t packetRxCount;
    uint16_t packetTxCount;
    float latitude;
    float longitude;
} AccessPoint;

typedef struct {
    FuriMessageQueue* queue;
    FuriMutex* mutex;
    FuriString* buffer;
    FuriString* buffer2;
    NotificationApp* notifications;
    FuriThread* thread_esp;
    FuriStreamBuffer* rx_stream_esp;
    uint8_t rx_buf_esp[2048];
    FuriThread* thread_gps;
    FuriStreamBuffer* rx_stream_gps;
    uint8_t rx_buf_gps[2048];
    File* file;
    char* dataString;
    uint16_t access_points_count;
    AccessPoint access_points[MAX_ACCESS_POINTS];
    int16_t access_points_index;
    AccessPoint active_access_point;
    bool extra_info;
    bool pressedButton;
    float last_latitude;
    float last_longitude;
} Context;

static void tick_callback(void* ctx_q) {
    furi_assert(ctx_q);
    FuriMessageQueue* queue = ctx_q;
    Event event = {.type = EventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void show_access_point(Canvas* canvas, Context* context) {
    Context* ctx = context;

    AccessPoint ap = ctx->active_access_point;

    canvas_draw_str_aligned(canvas, 62, 25, AlignCenter, AlignBottom, ap.ssid);

    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(
        canvas, 38 + (ctx->access_points_count > 99 ? 5 : 0), 12, AlignLeft, AlignBottom, ap.bssid);

    furi_string_printf(ctx->buffer, "Signal strength: %ddBm", ap.rssi);
    canvas_draw_str_aligned(
        canvas, 3, 35, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

    furi_string_printf(ctx->buffer, "CH: %d", ap.channel);
    canvas_draw_str_aligned(
        canvas, 3, 47, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

    if(ap.latitude == 0 && ap.longitude == 0) {
        canvas_draw_str_aligned(canvas, 29, 47, AlignLeft, AlignBottom, "X");
    } else {
        canvas_draw_str_aligned(canvas, 29, 47, AlignLeft, AlignBottom, "O");
    }

    furi_string_printf(ctx->buffer, "%d", ap.packetRxCount);
    canvas_draw_icon(canvas, 35, 39, &I_down);
    canvas_draw_str_aligned(
        canvas, 45, 47, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

    furi_string_printf(ctx->buffer, "%d", ap.packetTxCount);
    canvas_draw_icon(canvas, 85, 38, &I_up);
    canvas_draw_str_aligned(
        canvas, 95, 47, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

    furi_string_printf(
        ctx->buffer,
        "Seen: %d:%d:%d (%lds ago)",
        ap.datetime.hour,
        ap.datetime.minute,
        ap.datetime.second,
        furi_hal_rtc_get_timestamp() - furi_hal_rtc_datetime_to_timestamp(&ap.datetime));
    canvas_draw_str_aligned(
        canvas, 3, 59, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));
}

static void render_callback(Canvas* canvas, void* context) {
    Context* ctx = context;

    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_set_font(canvas, FontPrimary);

    if(ctx->access_points_count >= MAX_ACCESS_POINTS) {
        canvas_draw_str(canvas, 118, 10, "!");
    }

    if(ctx->access_points_count == 0) {
        canvas_draw_str(canvas, 80, 30, "No AP's");
        canvas_draw_str(canvas, 80, 40, "Found!");
        canvas_draw_icon(canvas, 1, 4, &I_DolphinWait_61x59);
    } else {
        canvas_draw_frame(canvas, 0, 0, 35 + (ctx->access_points_count > 99 ? 5 : 0), 15);

        furi_string_printf(
            ctx->buffer, "%d/%d", ctx->access_points_index + 1, ctx->access_points_count);

        canvas_draw_str(canvas, 3, 12, furi_string_get_cstr(ctx->buffer));

        show_access_point(canvas, ctx);
    }
    // canvas_clear(canvas);
    furi_mutex_release(ctx->mutex);
}

// order ctx->access_points by ssid alphabetically
static void sort_access_points(Context* ctx) {
    for(int i = 0; i < ctx->access_points_count; i++) {
        for(int j = i + 1; j < ctx->access_points_count; j++) {
            if(strcmp(ctx->access_points[i].ssid, ctx->access_points[j].ssid) > 0) {
                AccessPoint temp = ctx->access_points[i];
                ctx->access_points[i] = ctx->access_points[j];
                ctx->access_points[j] = temp;
            }
        }
    }
}

// set the index from the active access point
static void set_index_from_access_points(Context* ctx) {
    for(int i = 0; i < ctx->access_points_count; i++) {
        if(ctx->access_points[i].bssid == ctx->active_access_point.bssid) {
            ctx->access_points_index = i;
            break;
        }
    }
}

static void removeSpaces(char* str) {
    // Remove spaces from the beginning of the string
    int i = 0;
    while(isspace((unsigned char)str[i])) {
        i++;
    }

    // Move the remaining characters to the beginning of the string
    int j = 0;
    while(str[i] != '\0') {
        str[j++] = str[i++];
    }
    str[j] = '\0';

    // Remove spaces from the end of the string
    int len = strlen(str);
    while(len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

static void parseLine(void* context, char* line) {
    Context* ctx = context;

    AccessPoint ap = {.ssid = malloc(MAX_SSID_LENGTH + 1), .bssid = malloc(MAX_BSSID_LENGTH + 1)};

    Packet pkt = {.recievedMac = malloc(18 + 1), .sentMac = malloc(18 + 1)};

    char* token = strtok(line, ",");
    int i = 0;
    bool isAp = false;
    bool isValid = true;
    UNUSED(isValid);
    while(token != NULL) {
        switch(i) {
        case 0:
            if(strcmp(token, "AR") == 0) {
                isAp = true;
                isValid = true;
            } else if(strcmp(token, "PK") == 0) {
                isAp = false;
                isValid = true;
            }
            break;
        case 1:
            if(isAp && isValid) {
                removeSpaces(token);
                strcpy(ap.ssid, token);
            } else if(!isAp && isValid) {
                strncpy(pkt.recievedMac, token, 18);
                pkt.recievedMac[18] = '\0';
            }
            break;
        case 2:
            if(isAp && isValid) {
                strcpy(ap.bssid, token);
            } else if(!isAp && isValid) {
                strncpy(pkt.sentMac, token, 18);
                pkt.sentMac[18] = '\0';
            }
            break;
        case 3:
            if(isAp && isValid) {
                ap.rssi = atoi(token);
            }
            break;
        case 4:
            if(isAp && isValid) {
                ap.channel = atoi(token);
            }
            break;
        }

        token = strtok(NULL, ",");
        i++;
    }

    if(isAp && isValid) {
        // free the packet
        free(pkt.recievedMac);
        free(pkt.sentMac);

        // check if values are valid
        // bssid needs an ":"
        // rssi needs to be negative
        // channel needs to be between 1 and 14
        // ssid needs to be at least 1 character long
        if(ap.bssid[2] != ':' || ap.bssid[5] != ':' || ap.bssid[8] != ':' || ap.bssid[11] != ':' ||
           ap.bssid[14] != ':' || ap.rssi > 0 || ap.channel < 1 || ap.channel > 14 ||
           strlen(ap.ssid) < 1) {
            free(ap.ssid);
            free(ap.bssid);
            return;
        }

        furi_hal_light_set(LightBlue, 0);
        furi_hal_light_set(LightGreen, 255);

        furi_hal_rtc_get_datetime(&ap.datetime);

        if(isnan(ctx->last_latitude) || isnan(ctx->last_longitude)) {
            ctx->last_latitude = 0;
            ctx->last_longitude = 0;
        } else {
            ap.latitude = ctx->last_latitude;
            ap.longitude = ctx->last_longitude;
        }

        // check if ap is already in the list otherwise add it but update the rssi
        bool found = false;
        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, ap.bssid) == 0) {
                found = true;
                //update rssi channel datetime
                ctx->access_points[i].rssi = ap.rssi;
                ctx->access_points[i].channel = ap.channel;
                ctx->access_points[i].datetime = ap.datetime;
                ctx->access_points[i].latitude = ap.latitude;
                ctx->access_points[i].longitude = ap.longitude;

                if(strcmp(ctx->active_access_point.bssid, ap.bssid) == 0) {
                    ctx->active_access_point.rssi = ap.rssi;
                    ctx->active_access_point.channel = ap.channel;
                    ctx->active_access_point.datetime = ap.datetime;
                    ctx->active_access_point.latitude = ap.latitude;
                    ctx->active_access_point.longitude = ap.longitude;
                }

                free(ap.ssid);
                free(ap.bssid);

                break;
            }
        }

        if(!found) {
            memcpy(&ctx->access_points[ctx->access_points_count], &ap, sizeof(AccessPoint));
            ctx->access_points_count++;
        }

        sort_access_points(ctx);
        set_index_from_access_points(ctx);
    } else {
        // it is a packet so screw the ap
        free(ap.ssid);
        free(ap.bssid);

        // check if values are valid
        // mac needs to be 6 characters long
        if(strlen(pkt.recievedMac) != 17 || strlen(pkt.sentMac) != 17 ||
           ctx->access_points_count == 0) {
            free(pkt.recievedMac);
            free(pkt.sentMac);
            return;
        }

        furi_hal_light_set(LightGreen, 0);
        furi_hal_light_set(LightBlue, 255);

        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, pkt.recievedMac) == 0) {
                ctx->access_points[i].packetRxCount++;
                break;
            }
        }

        for(size_t i = 0; i < ctx->access_points_count; i++) {
            if(strcmp(ctx->access_points[i].bssid, pkt.sentMac) == 0) {
                ctx->access_points[i].packetTxCount++;
                break;
            }
        }

        free(pkt.recievedMac);
        free(pkt.sentMac);
    }
}

static void uart_cb_esp(UartIrqEvent ev, uint8_t data, void* context) {
    Context* ctx = (Context*)context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(ctx->rx_stream_esp, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_esp), WorkerEvtRxDone);
    }
}

static int32_t uart_worker_esp(void* context) {
    Context* ctx = (Context*)context;

    size_t rx_offset = 0;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) {
            break;
        }

        if(events & WorkerEvtRxDone) {
            size_t len = 0;
            do {
                // receive serial bytes into rx_buf, starting at rx_offset from the start of the buffer
                // the maximum we can receive is RX_BUF_SIZE - 1 - rx_offset
                len = furi_stream_buffer_receive(
                    ctx->rx_stream_esp,
                    ctx->rx_buf_esp + rx_offset,
                    RX_BUF_SIZE - 1 - rx_offset,
                    0);

                if(len > 0) {
                    // increase rx_offset by the number of bytes received, and null-terminate rx_buf
                    rx_offset += len;
                    ctx->rx_buf_esp[rx_offset] = '\0';

                    // look for strings ending in newlines, starting at the start of rx_buf
                    char* line_current = (char*)ctx->rx_buf_esp;
                    while(1) {
                        // skip null characters
                        while(*line_current == '\0' &&
                              line_current < (char*)ctx->rx_buf_esp + rx_offset - 1) {
                            line_current++;
                        }

                        // find the next newline
                        char* newline = strchr(line_current, '\n');
                        if(newline) // newline found
                        {
                            // put a null terminator in place of the newline, to delimit the line string
                            *newline = '\0';

                            parseLine(ctx, line_current);

                            // move the cursor to the character after the newline
                            line_current = newline + 1;
                        } else // no more newlines found
                        {
                            if(line_current >
                               (char*)ctx->rx_buf_esp) // at least one line was found
                            {
                                // clear parsed lines, and move any leftover bytes to the start of rx_buf
                                rx_offset = 0;
                                while(
                                    *line_current) // stop when the original rx_offset terminator is reached
                                {
                                    ctx->rx_buf_esp[rx_offset++] = *(line_current++);
                                }
                            }
                            break; // go back to receiving bytes from the serial stream
                        }
                    }
                }
            } while(len > 0);
        }
    }

    furi_hal_uart_set_irq_cb(UART_CH_ESP, NULL, NULL);

    furi_stream_buffer_free(ctx->rx_stream_esp);

    return 0;
}

static void gps_uart_parse_nmea(Context* ctx, char* line) {
    switch(minmea_sentence_id(line, false)) {
    case MINMEA_SENTENCE_RMC: {
        struct minmea_sentence_rmc frame;
        if(minmea_parse_rmc(&frame, line)) {
            ctx->last_latitude = minmea_tocoord(&frame.latitude);
            ctx->last_longitude = minmea_tocoord(&frame.longitude);
        }
    } break;

    case MINMEA_SENTENCE_GGA: {
        struct minmea_sentence_gga frame;
        if(minmea_parse_gga(&frame, line)) {
            ctx->last_latitude = minmea_tocoord(&frame.latitude);
            ctx->last_longitude = minmea_tocoord(&frame.longitude);
        }
    } break;

    case MINMEA_SENTENCE_GLL: {
        struct minmea_sentence_gll frame;
        if(minmea_parse_gll(&frame, line)) {
            ctx->last_latitude = minmea_tocoord(&frame.latitude);
            ctx->last_longitude = minmea_tocoord(&frame.longitude);
        }
    } break;

    default:
        break;
    }
}

static void uart_cb_gps(UartIrqEvent ev, uint8_t data, void* context) {
    Context* ctx = (Context*)context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(ctx->rx_stream_gps, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_gps), WorkerEvtRxDone);
    }
}

static int32_t uart_worker_gps(void* context) {
    Context* ctx = (Context*)context;

    size_t rx_offset = 0;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_ALL_RX_EVENTS, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEvtStop) {
            break;
        }

        if(events & WorkerEvtRxDone) {
            size_t len = 0;
            do {
                // receive serial bytes into rx_buf, starting at rx_offset from the start of the buffer
                // the maximum we can receive is RX_BUF_SIZE - 1 - rx_offset
                len = furi_stream_buffer_receive(
                    ctx->rx_stream_gps,
                    ctx->rx_buf_gps + rx_offset,
                    RX_BUF_SIZE - 1 - rx_offset,
                    0);

                if(len > 0) {
                    // increase rx_offset by the number of bytes received, and null-terminate rx_buf
                    rx_offset += len;
                    ctx->rx_buf_gps[rx_offset] = '\0';

                    // look for strings ending in newlines, starting at the start of rx_buf
                    char* line_current = (char*)ctx->rx_buf_gps;
                    while(1) {
                        // skip null characters
                        while(*line_current == '\0' &&
                              line_current < (char*)ctx->rx_buf_gps + rx_offset - 1) {
                            line_current++;
                        }

                        // find the next newline
                        char* newline = strchr(line_current, '\n');
                        if(newline) // newline found
                        {
                            // put a null terminator in place of the newline, to delimit the line string
                            *newline = '\0';

                            // FURI_LOG_I(appname, "Received line: %s", line_current);

                            gps_uart_parse_nmea(ctx, line_current);

                            // move the cursor to the character after the newline
                            line_current = newline + 1;
                        } else // no more newlines found
                        {
                            if(line_current >
                               (char*)ctx->rx_buf_gps) // at least one line was found
                            {
                                // clear parsed lines, and move any leftover bytes to the start of rx_buf
                                rx_offset = 0;
                                while(
                                    *line_current) // stop when the original rx_offset terminator is reached
                                {
                                    ctx->rx_buf_gps[rx_offset++] = *(line_current++);
                                }
                            }
                            break; // go back to receiving bytes from the serial stream
                        }
                    }
                }
            } while(len > 0);
        }
    }

    furi_hal_uart_set_irq_cb(UART_CH_GPS, NULL, NULL);

    furi_stream_buffer_free(ctx->rx_stream_gps);

    return 0;
}

int32_t wifisniffer_app(void* p) {
    UNUSED(p);

    // if(UART_CH_ESP == UART_CH_GPS) {
    //     FURI_LOG_I(appname, "ESP and GPS uart can't be the same");
    //     return -1;
    // }

    // turn off 5v, so it gets reset on startup
    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    // Enable 5v on startup
    uint8_t attempts = 0;
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    furi_delay_ms(200);

    // alloc everything
    Context* ctx = malloc(sizeof(Context));
    ctx->queue = furi_message_queue_alloc(8, sizeof(Event));
    ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ctx->buffer = furi_string_alloc();
    ctx->buffer2 = furi_string_alloc();
    ctx->notifications = furi_record_open(RECORD_NOTIFICATION);

    ctx->access_points_count = 0;
    ctx->access_points_index = 0;

    ctx->pressedButton = false;

    //esp uart
    ctx->rx_stream_esp = furi_stream_buffer_alloc(RX_BUF_SIZE * 5, 1);

    ctx->thread_esp = furi_thread_alloc();
    furi_thread_set_name(ctx->thread_esp, "LLwifiSnifferUartWorkerESP");
    furi_thread_set_stack_size(ctx->thread_esp, 2048);
    furi_thread_set_context(ctx->thread_esp, ctx);
    furi_thread_set_callback(ctx->thread_esp, uart_worker_esp);

    furi_thread_start(ctx->thread_esp);

    if(UART_CH_ESP == FuriHalUartIdUSART1) {
        furi_hal_console_disable();
    } else if(UART_CH_ESP == FuriHalUartIdLPUART1) {
        furi_hal_uart_init(UART_CH_ESP, 115200);
    }
    furi_hal_uart_set_br(UART_CH_ESP, 115200);
    furi_hal_uart_set_irq_cb(UART_CH_ESP, uart_cb_esp, ctx);

    // furi_hal_uart_tx(UART_CH_ESP, (uint8_t*)"CFW#WIFISNIFF=1\r\n", strlen("CFW#WIFISNIFF=1\r\n"));
    //end esp uart

    //gps uart
    if(UART_CH_ESP != UART_CH_GPS) {
        ctx->rx_stream_gps = furi_stream_buffer_alloc(RX_BUF_SIZE * 5, 1);

        ctx->thread_gps = furi_thread_alloc();
        furi_thread_set_name(ctx->thread_gps, "LLwifiSnifferUartWorkerGPS");
        furi_thread_set_stack_size(ctx->thread_gps, 2048);
        furi_thread_set_context(ctx->thread_gps, ctx);
        furi_thread_set_callback(ctx->thread_gps, uart_worker_gps);

        furi_thread_start(ctx->thread_gps);

        if(UART_CH_GPS == FuriHalUartIdUSART1) {
            furi_hal_console_disable();
        } else if(UART_CH_GPS == FuriHalUartIdLPUART1) {
            furi_hal_uart_init(UART_CH_GPS, 9600);
        }
        furi_hal_uart_set_br(UART_CH_GPS, 9600);
        furi_hal_uart_set_irq_cb(UART_CH_GPS, uart_cb_gps, ctx);
    }
    //end gps uart

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, ctx);
    view_port_input_callback_set(view_port, input_callback, ctx->queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(tick_callback, FuriTimerTypePeriodic, ctx->queue);
    furi_timer_start(timer, 100);

    // application loop
    Event event;
    bool processing = true;
    do {
        if(furi_message_queue_get(ctx->queue, &event, FuriWaitForever) == FuriStatusOk) {
            furi_mutex_acquire(ctx->mutex, FuriWaitForever);
            switch(event.type) {
            case EventTypeKey:
                // applicatie verlaten
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    processing = false;
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyBack) {
                    processing = false;
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyOk) {
                    // remove accespoint
                    if(ctx->access_points_count > 0) {
                        for(int i = ctx->access_points_index; i < ctx->access_points_count - 1;
                            i++) {
                            ctx->access_points[i] = ctx->access_points[i + 1];
                        }
                        ctx->access_points_count--;
                        if(ctx->access_points_index >= ctx->access_points_count) {
                            ctx->access_points_index = ctx->access_points_count - 1;
                        }
                    }
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyDown) {
                    ctx->access_points_index--;
                    if(ctx->access_points_index < 0) {
                        ctx->access_points_index = ctx->access_points_count - 1;
                    }
                    ctx->active_access_point = ctx->access_points[ctx->access_points_index];
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyUp) {
                    ctx->access_points_index++;
                    if(ctx->access_points_index >= ctx->access_points_count) {
                        ctx->access_points_index = 0;
                    }
                    ctx->active_access_point = ctx->access_points[ctx->access_points_index];
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyLeft) {
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyRight) {
                }
                ctx->pressedButton = true;
                break;
            case EventTypeTick:

                // fix for the empty active access point when there was no interaction
                if(!ctx->pressedButton) {
                    ctx->access_points_index = 0;
                    ctx->active_access_point = ctx->access_points[ctx->access_points_index];
                }

                break;

            default:
                break;
            }

            view_port_update(view_port);
        } else {
            processing = false;
        }
    } while(processing);

    // save the data to the file
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);

    FuriString* filename = furi_string_alloc();
    furi_string_printf(
        filename,
        "%d_%d_%d_%d_%d_%d.txt",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);

    FuriString* path = furi_string_alloc();
    furi_string_printf(path, "/ext/apps_data/llsniffer/%s", furi_string_get_cstr(filename));

    // open file
    ctx->file = storage_file_alloc(storage);

    if(!storage_common_exists(storage, EXT_PATH("apps_data/llsniffer"))) {
        storage_common_mkdir(storage, EXT_PATH("apps_data/llsniffer"));
    }

    if(!storage_file_open(ctx->file, furi_string_get_cstr(path), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        FURI_LOG_E(appname, "Failed to open file");
    }

    for(int i = 0; i < ctx->access_points_count; i++) {
        AccessPoint ap = ctx->access_points[i];
        furi_string_printf(
            ctx->buffer2,
            "%s,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f\r\n",
            "Accesspoint",
            ap.ssid,
            ap.bssid,
            ap.rssi,
            ap.channel,
            ap.datetime.year,
            ap.datetime.month,
            ap.datetime.day,
            ap.datetime.hour,
            ap.datetime.minute,
            ap.datetime.second,
            (double)ap.latitude,
            (double)ap.longitude);

        if(!storage_file_write(
               ctx->file,
               furi_string_get_cstr(ctx->buffer2),
               strlen(furi_string_get_cstr(ctx->buffer2)))) {
            FURI_LOG_E(appname, "Failed to write AP to file");
        }
    }

    // free everything
    furi_record_close(RECORD_NOTIFICATION);
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(ctx->queue);
    furi_mutex_free(ctx->mutex);

    furi_thread_flags_set(furi_thread_get_id(ctx->thread_esp), WorkerEvtStop);
    furi_thread_join(ctx->thread_esp);
    furi_thread_free(ctx->thread_esp);

    if(UART_CH_ESP != UART_CH_GPS) {
        furi_thread_flags_set(furi_thread_get_id(ctx->thread_gps), WorkerEvtStop);
        furi_thread_join(ctx->thread_gps);
        furi_thread_free(ctx->thread_gps);
    }

    storage_file_close(ctx->file);
    storage_file_free(ctx->file);
    furi_record_close(RECORD_STORAGE);
    free(ctx);

    furi_hal_light_set(LightBlue, 0);
    furi_hal_light_set(LightGreen, 0);

    if(UART_CH_ESP == FuriHalUartIdLPUART1) {
        furi_hal_uart_deinit(UART_CH_ESP);
    } else if(UART_CH_ESP == FuriHalUartIdUSART1) {
        furi_hal_console_enable();
    }

    if(UART_CH_GPS == FuriHalUartIdLPUART1) {
        furi_hal_uart_deinit(UART_CH_GPS);
    } else if(UART_CH_GPS == FuriHalUartIdUSART1) {
        furi_hal_console_enable();
    }

    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    return 0;
}
