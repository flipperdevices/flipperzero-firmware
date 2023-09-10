#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_port.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <locale/locale.h>

#include "max31855.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    FuriMutex* mutex;
    uint32_t data;
} Max31855App;

static void max31855_render_callback(Canvas* canvas, void* ctx) {
    Max31855App* state = ctx;
    char buffer[64];

    float tc_temp = max31855_unpack_temp(state->data);
    float chip_temp = max31855_unpack_internal_temp(state->data);

    if(locale_get_measurement_unit() == LocaleMeasurementUnitsImperial) {
        tc_temp = locale_celsius_to_fahrenheit(tc_temp);
        chip_temp = locale_celsius_to_fahrenheit(chip_temp);
    }

    furi_mutex_acquire(state->mutex, FuriWaitForever);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 15, "TC temp:");

    canvas_set_font(canvas, FontBigNumbers);
    snprintf(buffer, sizeof(buffer), "%0.2f", (double)tc_temp);
    canvas_draw_str(canvas, 50, 15, buffer);

    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, sizeof(buffer), "Chip temp: %0.2f", (double)chip_temp);
    canvas_draw_str(canvas, 0, 28, buffer);

    if(max31855_unpack_fault(state->data)) {
        snprintf(
            buffer,
            sizeof(buffer),
            "Fault: %s %s %s",
            max31855_unpack_oc(state->data) ? "OC" : "",
            max31855_unpack_scg(state->data) ? "SCG" : "",
            max31855_unpack_scv(state->data) ? "SCV" : "");
        canvas_draw_str(canvas, 0, 41, buffer);
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 63, "[back] - to exit");

    furi_mutex_release(state->mutex);
}

static void max31855_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(event_queue, input_event, FuriWaitForever);
    }
}

int32_t max31855_app(void* p) {
    UNUSED(p);

    Max31855App state = {0};
    state.gui = furi_record_open(RECORD_GUI);
    state.notification = furi_record_open(RECORD_NOTIFICATION);
    state.view_port = view_port_alloc();
    state.event_queue = furi_message_queue_alloc(32, sizeof(InputEvent));
    state.mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    view_port_draw_callback_set(state.view_port, max31855_render_callback, &state);
    view_port_input_callback_set(state.view_port, max31855_input_callback, state.event_queue);
    gui_add_view_port(state.gui, state.view_port, GuiLayerFullscreen);
    max31855_open();
    notification_message(state.notification, &sequence_display_backlight_enforce_on);

    InputEvent event;
    do {
        bool do_refresh = furi_message_queue_get(state.event_queue, &event, 1000) != FuriStatusOk;

        furi_mutex_acquire(state.mutex, FuriWaitForever);

        if(do_refresh) {
            state.data = max31855_read();
        } else if(event.key == InputKeyBack) {
            furi_mutex_release(state.mutex);
            break;
        }

        furi_mutex_release(state.mutex);
        view_port_update(state.view_port);
    } while(true);

    notification_message(state.notification, &sequence_display_backlight_enforce_auto);
    max31855_close();
    gui_remove_view_port(state.gui, state.view_port);

    furi_mutex_free(state.mutex);
    furi_message_queue_free(state.event_queue);
    view_port_free(state.view_port);
    state.notification = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    state.gui = NULL;
    furi_record_close(RECORD_GUI);

    return 0;
}
