#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_port.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "mhz19.h"

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Mhz19* mhz19;

    FuriMutex* mutex;
    uint32_t co2_ppm;
    uint32_t counter;
    bool is_connected;
} Mhz19Uart;

static void mhz19_uart_render_callback(Canvas* canvas, void* ctx) {
    Mhz19Uart* state = ctx;
    char buffer[64];

    furi_mutex_acquire(state->mutex, FuriWaitForever);

    canvas_set_font(canvas, FontSecondary);
    if(state->is_connected) {
        snprintf(buffer, sizeof(buffer), "Status: Online, %lus", state->counter);
    } else {
        snprintf(buffer, sizeof(buffer), "Status: Offline");
    }
    canvas_draw_str(canvas, 10, 10, buffer);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 10, 40, "CO2 PPM:");

    canvas_set_font(canvas, FontBigNumbers);
    snprintf(buffer, sizeof(buffer), "%lu", state->co2_ppm);
    canvas_draw_str(canvas, 70, 40, buffer);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 10, 63, "[back] - to exit");

    furi_mutex_release(state->mutex);
}

static void mhz19_uart_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(event_queue, input_event, FuriWaitForever);
    }
}

int32_t mhz19_uart_app(void* p) {
    UNUSED(p);

    Mhz19Uart state = {0};
    state.gui = furi_record_open(RECORD_GUI);
    state.notification = furi_record_open(RECORD_NOTIFICATION);
    state.view_port = view_port_alloc();
    state.event_queue = furi_message_queue_alloc(32, sizeof(InputEvent));
    state.mhz19 = mhz19_alloc();
    state.mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    view_port_draw_callback_set(state.view_port, mhz19_uart_render_callback, &state);
    view_port_input_callback_set(state.view_port, mhz19_uart_input_callback, state.event_queue);
    gui_add_view_port(state.gui, state.view_port, GuiLayerFullscreen);
    mhz19_open(state.mhz19);
    notification_message(state.notification, &sequence_display_backlight_enforce_on);

    InputEvent event;
    do {
        bool do_refresh = furi_message_queue_get(state.event_queue, &event, 1000) != FuriStatusOk;

        furi_mutex_acquire(state.mutex, FuriWaitForever);

        if(do_refresh) {
            state.is_connected = mhz19_read_gas_concentration(state.mhz19, &state.co2_ppm);
            if(state.is_connected) {
                state.counter++;
            } else {
                state.counter = 0;
            }
        } else if(event.key == InputKeyBack) {
            furi_mutex_release(state.mutex);
            break;
        }

        furi_mutex_release(state.mutex);
        view_port_update(state.view_port);
    } while(true);

    notification_message(state.notification, &sequence_display_backlight_enforce_auto);
    mhz19_close(state.mhz19);
    gui_remove_view_port(state.gui, state.view_port);

    furi_mutex_free(state.mutex);
    mhz19_free(state.mhz19);
    furi_message_queue_free(state.event_queue);
    view_port_free(state.view_port);
    state.notification = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    state.gui = NULL;
    furi_record_close(RECORD_GUI);

    return 0;
}
