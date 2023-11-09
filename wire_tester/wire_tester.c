#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <stdlib.h>

#include "wire_tester_icons.h"

//#define TAG "wire_tester"

static const uint32_t EVENT_PERIOD_MS = 10; // check for input changes often
static const float BEEP_FREQ = 2400.0f; // louder than other frequencies
static const float BEEP_VOL = 0.8f;
static const GpioPin* const INPUT_PIN = &gpio_ext_pb2; // pin 6

static void start_feedback(NotificationApp* notifications) {
    // set LED to green
    notification_message_block(notifications, &sequence_set_only_green_255);

    // start beep
    if(furi_hal_speaker_acquire(1000)) {
        furi_hal_speaker_start(BEEP_FREQ, BEEP_VOL);
    }
}

static void stop_feedback(NotificationApp* notifications) {
    // clear LED
    notification_message_block(notifications, &sequence_reset_rgb);

    // stop beep
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, &I_background_128x64);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t app_main(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // force backlight on because our hands will be busy with wires
    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notifications, &sequence_display_backlight_enforce_on);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, view_port);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    stop_feedback(notifications);

    // set input to be normally high; it will be low when shorted to ground
    furi_hal_gpio_init(INPUT_PIN, GpioModeInput, GpioPullUp, GpioSpeedLow);

    bool alarming = false;
    bool running = true;
    while(running) {
        // start and stop feedback on the transition
        bool continuous = !furi_hal_gpio_read(INPUT_PIN);
        if(continuous && !alarming) {
            start_feedback(notifications);
        } else if(!continuous && alarming) {
            stop_feedback(notifications);
        }
        alarming = continuous;

        // exit on back key
        InputEvent event;
        if(furi_message_queue_get(event_queue, &event, EVENT_PERIOD_MS) == FuriStatusOk) {
            if((event.type == InputTypePress || event.type == InputTypeRepeat) &&
               event.key == InputKeyBack) {
                running = false;
            }
        }
    }

    // return control of the LED, beeper, and backlight
    stop_feedback(notifications);
    notification_message_block(notifications, &sequence_display_backlight_enforce_auto);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);

    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    return 0;
}
