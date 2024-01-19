#include "gps_uart.h"
#include "constants.h"

#include <furi.h>
#include <gui/gui.h>
#include <string.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

static void render_callback(Canvas* const canvas, void* context) {
    furi_assert(context);
    GpsUart* gps_uart = context;
    furi_mutex_acquire(gps_uart->mutex, FuriWaitForever);

    char buffer[64];

    switch(gps_uart->view_state) {
    case CHANGE_BAUDRATE:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "Baudrate set to:");
        snprintf(buffer, 64, "%ld baud", gps_uart->baudrate);
        canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignBottom, buffer);
        break;
    case CHANGE_BACKLIGHT:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas,
            64,
            32,
            AlignCenter,
            AlignBottom,
            gps_uart->backlight_enabled ? "Backlight enabled" : "Backlight disabled");
        break;
    case CHANGE_DEEPSLEEP:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas,
            64,
            32,
            AlignCenter,
            AlignBottom,
            gps_uart->deep_sleep_enabled ? "Deep sleep enabled" : "Deep sleep disabled");
        break;
    case CHANGE_SPEEDUNIT:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "Speed unit set to:");
        switch(gps_uart->speed_units) {
        case KPH:
            canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignBottom, "km/h");
            break;
        case MPH:
            canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignBottom, "mi/h");
            break;
        case KNOTS:
        default:
            canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignBottom, "kn");
            break;
        }
        break;
    case NORMAL:
    default:
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 32, 8, AlignCenter, AlignBottom, "Latitude");
        canvas_draw_str_aligned(canvas, 96, 8, AlignCenter, AlignBottom, "Longitude");
        canvas_draw_str_aligned(canvas, 21, 30, AlignCenter, AlignBottom, "Course");
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignBottom, "Speed");
        canvas_draw_str_aligned(canvas, 107, 30, AlignCenter, AlignBottom, "Altitude");
        canvas_draw_str_aligned(canvas, 32, 52, AlignCenter, AlignBottom, "Satellites");
        canvas_draw_str_aligned(canvas, 96, 52, AlignCenter, AlignBottom, "Last Fix");

        canvas_set_font(canvas, FontSecondary);
        snprintf(buffer, 64, "%f", (double)gps_uart->status.latitude);
        canvas_draw_str_aligned(canvas, 32, 18, AlignCenter, AlignBottom, buffer);
        snprintf(buffer, 64, "%f", (double)gps_uart->status.longitude);
        canvas_draw_str_aligned(canvas, 96, 18, AlignCenter, AlignBottom, buffer);
        snprintf(buffer, 64, "%.1f", (double)gps_uart->status.course);
        canvas_draw_str_aligned(canvas, 21, 40, AlignCenter, AlignBottom, buffer);

        switch(gps_uart->speed_units) {
        case KPH:
            snprintf(buffer, 64, "%.2f km/h", (double)(gps_uart->status.speed * KNOTS_TO_KPH));
            break;
        case MPH:
            snprintf(buffer, 64, "%.2f mi/h", (double)(gps_uart->status.speed * KNOTS_TO_MPH));
            break;
        case KNOTS:
        default:
            snprintf(buffer, 64, "%.2f kn", (double)gps_uart->status.speed);
            break;
        }

        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, buffer);
        snprintf(
            buffer,
            64,
            "%.1f %c",
            (double)gps_uart->status.altitude,
            tolower(gps_uart->status.altitude_units));
        canvas_draw_str_aligned(canvas, 107, 40, AlignCenter, AlignBottom, buffer);
        snprintf(buffer, 64, "%d", gps_uart->status.satellites_tracked);
        canvas_draw_str_aligned(canvas, 32, 62, AlignCenter, AlignBottom, buffer);
        snprintf(
            buffer,
            64,
            "%02d:%02d:%02d UTC",
            gps_uart->status.time_hours,
            gps_uart->status.time_minutes,
            gps_uart->status.time_seconds);
        canvas_draw_str_aligned(canvas, 96, 62, AlignCenter, AlignBottom, buffer);
        break;
    }

    furi_mutex_release(gps_uart->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t gps_app(void* p) {
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    GpsUart* gps_uart = gps_uart_enable();

    gps_uart->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!gps_uart->mutex) {
        FURI_LOG_E("GPS", "cannot create mutex\r\n");
        free(gps_uart);
        return 255;
    }

    uint8_t attempts = 0;
    bool otg_was_enabled = furi_hal_power_is_otg_enabled();
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }
    furi_delay_ms(200);

    // set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, gps_uart);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(gps_uart->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyBack:
                        processing = false;
                        break;
                    case InputKeyOk:
                        if(!gps_uart->backlight_enabled) {
                            notification_message_block(
                                gps_uart->notifications, &sequence_display_backlight_enforce_on);
                            gps_uart->backlight_enabled = true;
                        } else {
                            notification_message_block(
                                gps_uart->notifications, &sequence_display_backlight_enforce_auto);
                            notification_message(
                                gps_uart->notifications, &sequence_display_backlight_off);
                            gps_uart->backlight_enabled = false;
                        }

                        gps_uart->view_state = CHANGE_BACKLIGHT;
                        furi_mutex_release(gps_uart->mutex);
                        view_port_update(view_port);
                        furi_delay_ms(1000);
                        gps_uart->view_state = NORMAL;
                        break;
                    default:
                        break;
                    }
                } else if(event.input.type == InputTypeLong) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        gps_uart_deinit_thread(gps_uart);
                        const int baudrate_length =
                            sizeof(gps_baudrates) / sizeof(gps_baudrates[0]);
                        current_gps_baudrate++;
                        if(current_gps_baudrate >= baudrate_length) {
                            current_gps_baudrate = 0;
                        }
                        gps_uart->baudrate = gps_baudrates[current_gps_baudrate];

                        gps_uart_init_thread(gps_uart);
                        gps_uart->view_state = CHANGE_BAUDRATE;

                        furi_mutex_release(gps_uart->mutex);
                        view_port_update(view_port);
                        furi_delay_ms(1000);
                        gps_uart->view_state = NORMAL;
                        break;
                    case InputKeyRight:
                        gps_uart->speed_units++;
                        if(gps_uart->speed_units == INVALID) {
                            gps_uart->speed_units = KNOTS;
                        }

                        gps_uart->view_state = CHANGE_SPEEDUNIT;
                        furi_mutex_release(gps_uart->mutex);
                        view_port_update(view_port);
                        furi_delay_ms(1000);
                        gps_uart->view_state = NORMAL;
                        break;
                    case InputKeyDown:
                        gps_uart->view_state = CHANGE_DEEPSLEEP;
                        gps_uart->deep_sleep_enabled = !gps_uart->deep_sleep_enabled;

                        // tested on Telit SE868-A and SL871L-S
                        furi_hal_serial_tx(
                            gps_uart->serial_handle,
                            (uint8_t*)"$PMTK161,0*28\r\n",
                            strlen("$PMTK161,0*28\r\n"));

                        furi_mutex_release(gps_uart->mutex);
                        view_port_update(view_port);
                        furi_delay_ms(1000);
                        gps_uart->view_state = NORMAL;
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if(gps_uart->view_state == NORMAL) {
            furi_mutex_release(gps_uart->mutex);
            view_port_update(view_port);
        }
    }

    notification_message_block(gps_uart->notifications, &sequence_display_backlight_enforce_auto);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(gps_uart->mutex);
    gps_uart_disable(gps_uart);

    if(furi_hal_power_is_otg_enabled() && !otg_was_enabled) {
        furi_hal_power_disable_otg();
    }

    return 0;
}
