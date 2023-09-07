// Created by @MatthewKuKanich for use with the RCWL-0516
// Design inspired from @unixispower Wire Tester

#include <furi_hal.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>
#include <gui/elements.h>

static const uint32_t EVENT_PERIOD_MS = 10;
static const float BEEP_FREQ = 1000.0f;
static const float BEEP_VOL = 0.9f;
static const GpioPin* const radarPin = &gpio_ext_pc3; // Pin 7

bool presenceDetected = false;
bool muted = false;
bool active = false;

static void start_feedback(NotificationApp* notifications) {
    // Set LED to red for detection
    notification_message_block(notifications, &sequence_set_only_red_255);

    // Set vibration
    notification_message_block(notifications, &sequence_double_vibro);

    if(!muted) {
        // Start beep if not muted
        if(furi_hal_speaker_acquire(1000)) {
            furi_hal_speaker_start(BEEP_FREQ, BEEP_VOL);
        }
    }
}

static void stop_feedback(NotificationApp* notifications) {
    // Clear LED
    notification_message_block(notifications, &sequence_reset_rgb);

    // Reset vibration
    notification_message_block(notifications, &sequence_reset_vibro);

    // Stop beeping
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "Microwave Radar");

    if(active) {
        elements_multiline_text_aligned(canvas, 64, 12, AlignCenter, AlignTop, "Active");
    } else {
        elements_multiline_text_aligned(canvas, 64, 12, AlignCenter, AlignTop, "On Standby");
    }

    // Display presence status
    if(presenceDetected) {
        elements_multiline_text_aligned(
            canvas, 64, 25, AlignCenter, AlignTop, "Presence Detected");
    } else {
        elements_multiline_text_aligned(canvas, 64, 25, AlignCenter, AlignTop, "No Presence");
    }

    if(muted) {
        elements_multiline_text_aligned(canvas, 64, 35, AlignCenter, AlignTop, "Muted");
    }

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 64, 45, AlignCenter, AlignTop, "RCWL-0516 :: OUT -> Pin7");
    elements_multiline_text_aligned(
        canvas, 64, 55, AlignCenter, AlignTop, "VIN -> 5v :: GND -> GND");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t app_radar_scanner(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // I'm keeping the forced backlight as you will likely be away from Flipper
    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notifications, &sequence_display_backlight_enforce_on);

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, view_port);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    view_port_update(view_port);

    stop_feedback(notifications);

    // set input to be low; RCWL-0516 outputs High (3v) on detection
    furi_hal_gpio_init(radarPin, GpioModeInput, GpioPullDown, GpioSpeedVeryHigh);

    bool alarming = false; // Sensor begins in-active until user starts
    bool running = true; // to prevent unwanted false positives

    while(running) {
        if(active) {
            // start and stop feedback if sensor state is active
            bool continuous = furi_hal_gpio_read(radarPin);

            if(continuous && !alarming) {
                presenceDetected = true;
                start_feedback(notifications);

            } else if(!continuous && alarming) {
                presenceDetected = false;
                stop_feedback(notifications); // Green LED if clear/no presence
                notification_message_block(notifications, &sequence_set_only_green_255);
            }
            alarming = continuous;
        }

        // Exit on back key
        InputEvent event;
        if(furi_message_queue_get(event_queue, &event, EVENT_PERIOD_MS) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                if(event.key == InputKeyBack) {
                    break;
                }
                if(event.key == InputKeyOk) {
                    active = !active; // Toggle the value of 'active'
                    stop_feedback(notifications);
                }
                if(event.key == InputKeyDown) {
                    muted = !muted; // Toggle the value of 'muted'
                    stop_feedback(notifications);
                }
            }
        }
    }

    // return control of the LED, beeper, backlight, and stop vibration
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
