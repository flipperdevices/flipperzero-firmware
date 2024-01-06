#include "wardriver.h"
#include "wardriver_uart.h"

void save_file(Context* ctx) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);

    FuriString* filename = furi_string_alloc();
    furi_string_printf(
        filename,
        "%s/%s_%d_%d_%d_%d_%d_%d.txt",
        FILE_PATH,
        "wigle",
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);

    File* file = storage_file_alloc(storage);

    if(!storage_common_exists(storage, FILE_PATH)) {
        storage_common_mkdir(storage, FILE_PATH);
    }

    if(!storage_file_open(file, furi_string_get_cstr(filename), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        FURI_LOG_I(appname, "Failed to open file");
        storage_file_close(file);
        storage_file_free(file);
        furi_string_free(filename);
        furi_record_close(RECORD_STORAGE);

        furi_hal_light_blink_start(LightRed, 100, 100, 5000);

        return;
    }

    furi_string_reset(ctx->buffer);

    // WIGLE HEADERS DONT CHANGE THIS ITS IMPORTANT!
    furi_string_printf(
        ctx->buffer,
        "%s,%s,%s,%s,%s,%s,%s,%s\r\n",
        "WigleWifi-1.4",
        "appRelease=v2.0",
        "model=S33",
        "release=XtremeFW",
        "Flipper Zero",
        "",
        "Wardriver",
        "S33");

    furi_string_cat_printf(
        ctx->buffer,
        "%s,%s,%s,%s,%s,%s,%s\r\n",
        "MAC",
        "SSID",
        "FirstSeen",
        "Channel",
        "RSSI",
        "CurrentLatitude",
        "CurrentLongitude");
    if(!storage_file_write(
           file, furi_string_get_cstr(ctx->buffer), strlen(furi_string_get_cstr(ctx->buffer)))) {
        FURI_LOG_I(appname, "Failed to write header to file");
    }

    for(int i = 0; i < ctx->access_points_count; i++) {
        AccessPoint ap = ctx->access_points[i];
        furi_string_printf(
            ctx->buffer,
            "%s,%s,%04d-%02d-%02d %02d:%02d:%02d,%d,%d,%f,%f\r\n",
            ap.bssid,
            ap.ssid,
            ap.datetime.year,
            ap.datetime.month,
            ap.datetime.day,
            ap.datetime.hour,
            ap.datetime.minute,
            ap.datetime.second,
            ap.rssi,
            ap.channel,
            (double)ap.latitude,
            (double)ap.longitude);

        if(!storage_file_write(
               file,
               furi_string_get_cstr(ctx->buffer),
               strlen(furi_string_get_cstr(ctx->buffer)))) {
            FURI_LOG_I(appname, "Failed to write AP to file");
        }

        free(ap.ssid);
        free(ap.bssid);
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_string_free(filename);
    furi_record_close(RECORD_STORAGE);

    return;
}

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

static void draw_access_point(Canvas* canvas, Context* context) {
    Context* ctx = context;

    AccessPoint ap = ctx->active_access_point;

    canvas_draw_str_aligned(canvas, 62, 25, AlignCenter, AlignBottom, ap.ssid);

    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 38, 12, AlignLeft, AlignBottom, ap.bssid);

    furi_string_printf(ctx->buffer, "Signal strength: %ddBm", ap.rssi);
    canvas_draw_str_aligned(
        canvas, 3, 35, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

    furi_string_printf(ctx->buffer, "CH: %d", ap.channel);
    canvas_draw_str_aligned(
        canvas, 3, 47, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));

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
        "Seen: %02d:%02d:%02d (%lds ago)",
        ap.datetime.hour,
        ap.datetime.minute,
        ap.datetime.second,
        furi_hal_rtc_get_timestamp() - furi_hal_rtc_datetime_to_timestamp(&ap.datetime));
    canvas_draw_str_aligned(
        canvas, 3, 59, AlignLeft, AlignBottom, furi_string_get_cstr(ctx->buffer));
}

static void render_callback(Canvas* canvas, void* context) {
    Context* ctx = context;

    canvas_set_font(canvas, FontPrimary);

    if(ctx->access_points_count >= MAX_ACCESS_POINTS) {
        canvas_draw_str(canvas, 118, 10, "!");
    }

    switch(ctx->view_state) {
    case SHOW_NMEA:

        if(UART_CH_ESP == UART_CH_GPS) {
            canvas_draw_str(canvas, 0, 10, "GPS channel invalid!");
            canvas_draw_str(canvas, 0, 20, "Change UART");
            canvas_draw_str(canvas, 0, 30, "channel");
            canvas_draw_str(canvas, 0, 40, "in the Xtreme");
            canvas_draw_str(canvas, 0, 50, "app");
        } else {
            furi_string_printf(
                ctx->buffer,
                "%f",
                isnan(ctx->gps_data.latitude) ? 0 : (double)ctx->gps_data.latitude);
            canvas_draw_str(canvas, 0, 10, furi_string_get_cstr(ctx->buffer));

            furi_string_printf(
                ctx->buffer,
                "%f",
                isnan(ctx->gps_data.longitude) ? 0 : (double)ctx->gps_data.longitude);
            canvas_draw_str(canvas, 0, 20, furi_string_get_cstr(ctx->buffer));

            furi_string_printf(ctx->buffer, "%d sats", ctx->gps_data.satelites);
            canvas_draw_str(canvas, 0, 30, furi_string_get_cstr(ctx->buffer));

            furi_string_printf(
                ctx->buffer,
                "%02d:%02d:%02dZ",
                ctx->gps_data.hour,
                ctx->gps_data.minute,
                ctx->gps_data.second);
            canvas_draw_str(canvas, 0, 40, furi_string_get_cstr(ctx->buffer));
            canvas_draw_str(canvas, 70, 10, "GPS DATA");
        }

        elements_button_left(canvas, "Back");

        canvas_draw_icon(canvas, 82, 20, &I_WarningDolphinFlip_45x42);
        break;
    case NO_APS:
        canvas_draw_str(canvas, 80, 30, "No AP's");
        canvas_draw_str(canvas, 80, 40, "Found!");
        canvas_draw_icon(canvas, 1, 4, &I_DolphinWait_61x59);
        break;
    case NORMAL:
    default:
        canvas_draw_frame(canvas, 0, 0, 128, 64);

        furi_string_printf(
            ctx->buffer, "%d/%d", ctx->access_points_index + 1, ctx->access_points_count);

        canvas_draw_str(canvas, 3, 12, furi_string_get_cstr(ctx->buffer));

        draw_access_point(canvas, ctx);
        break;
    }
    furi_mutex_release(ctx->mutex);
}

int32_t wardriver_app() {
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

    Context* ctx = malloc(sizeof(Context));
    ctx->queue = furi_message_queue_alloc(8, sizeof(Event));
    ctx->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ctx->buffer = furi_string_alloc();

    ctx->access_points_count = 0;
    ctx->access_points_index = 0;
    ctx->pressedButton = false;
    ctx->view_state = NO_APS;

    wardriver_uart_init(ctx);

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
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    if(ctx->view_state == SHOW_NMEA) {
                        ctx->view_state = NORMAL;
                    } else {
                        processing = false;
                    }
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyBack) {
                    if(ctx->view_state == SHOW_NMEA) {
                        ctx->view_state = NORMAL;
                    } else {
                        processing = false;
                    }
                } else if(event.input.type == InputTypeLong && event.input.key == InputKeyOk) {
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyDown) {
                    ctx->access_points_index--;
                    if(ctx->access_points_index < 0) {
                        ctx->access_points_index = ctx->access_points_count - 1;
                    }
                    ctx->active_access_point = ctx->access_points[ctx->access_points_index];
                    ctx->pressedButton = true;
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyUp) {
                    ctx->access_points_index++;
                    if(ctx->access_points_index >= ctx->access_points_count) {
                        ctx->access_points_index = 0;
                    }
                    ctx->active_access_point = ctx->access_points[ctx->access_points_index];
                    ctx->pressedButton = true;
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyLeft) {
                    if(ctx->view_state == NORMAL) {
                        ctx->view_state = SHOW_NMEA;
                    } else if(ctx->view_state == SHOW_NMEA) {
                        ctx->view_state = NORMAL;
                    }
                } else if(event.input.type == InputTypePress && event.input.key == InputKeyRight) {
                }
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

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    furi_message_queue_free(ctx->queue);
    furi_mutex_free(ctx->mutex);
    furi_string_free(ctx->buffer);

    wardriver_uart_deinit(ctx);

    save_file(ctx);

    free(ctx);

    furi_hal_light_set(LightBlue, 0);
    furi_hal_light_set(LightGreen, 0);

    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    return 0;
}