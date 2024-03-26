#include "test_midi.h"
#include "usb/usb_midi_driver.h"
#include "midi/usb_message.h"

#define MIDI_NOTE_ON_MSG 0x90
#define MIDI_NOTE_OFF_MSG 0x80
#define MIDI_VELOCITY 0x7F

void test_midi_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Flipper Midi Ocarina.");
    canvas_draw_str(canvas, 2, 30, "Press to send MIDI. ");
    canvas_draw_str(canvas, 2, 50, "Press Back to Exit. ");
}

void test_midi_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    TestMidiApp* app = ctx;

    uint8_t midi_data[4];
    midi_data[0] = 0x09;
    midi_data[3] = MIDI_VELOCITY;

    if(input_event->type == InputTypePress || input_event->type == InputTypeRelease) {
        if(input_event->key == InputKeyOk) {
            midi_data[2] = 0x48; // C5
        } else if(input_event->key == InputKeyLeft) {
            midi_data[2] = 0x4B; // D#5
        } else if(input_event->key == InputKeyUp) {
            midi_data[2] = 0x4D; // F5
        } else if(input_event->key == InputKeyRight) {
            midi_data[2] = 0x4F; // G5
        } else if(input_event->key == InputKeyDown) {
            midi_data[2] = 0x52; // A#5
        } else if(input_event->key == InputKeyBack) {
            //view_port_enabled_set(app->view_port, false);
            //return;
            app->should_exit = true;
            return;
        } else {
            return;
        }

        if(input_event->type == InputTypePress) {
            midi_data[1] = MIDI_NOTE_ON_MSG;
        } else if(input_event->type == InputTypeRelease) {
            midi_data[1] = MIDI_NOTE_OFF_MSG;
        }

        midi_usb_tx(midi_data, 4);
    }
}

TestMidiApp* test_midi_alloc() {
    TestMidiApp* app = malloc(sizeof(TestMidiApp));
    app->midi_note = 0x48; // C5
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, test_midi_draw_callback, app);
    view_port_input_callback_set(app->view_port, test_midi_input_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    //app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->should_exit = false;
    return app;
}

void test_midi_free(TestMidiApp* app) {
    furi_assert(app);

    //furi_record_close(RECORD_NOTIFICATION);
    //app->notifications = NULL;

    // Free GUI
    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    // Free viewport
    view_port_free(app->view_port);

    // Free rest
    free(app);
}

int32_t test_midi_app() {
    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&midi_usb_interface, NULL);
    furi_hal_usb_lock();

    TestMidiApp* app = test_midi_alloc();

    while(!app->should_exit) {
        view_port_update(app->view_port);
        furi_thread_flags_wait(0, FuriFlagWaitAny, 50);
    }

    test_midi_free(app);

    furi_hal_usb_unlock();
    furi_hal_usb_set_config(usb_mode_prev, NULL);

    return 0;
}