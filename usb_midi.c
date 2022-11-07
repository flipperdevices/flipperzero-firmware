#include <furi.h>
#include "usb/usb_midi_driver.h"

typedef enum {
    MidiThreadEventStop = (1 << 0),
    MidiThreadEventRx = (1 << 1),
    MidiThreadEventAll = MidiThreadEventStop | MidiThreadEventRx,
} MidiThreadEvent;

static void midi_rx_callback(void* context) {
    furi_assert(context);
    FuriThreadId thread_id = (FuriThreadId)context;
    furi_thread_flags_set(thread_id, MidiThreadEventRx);
}

int32_t usb_midi_app(void* p) {
    UNUSED(p);

    FuriHalUsbInterface* usb_config_prev;
    usb_config_prev = furi_hal_usb_get_config();
    midi_usb_set_context(furi_thread_get_id(furi_thread_get_current()));
    midi_usb_set_rx_callback(midi_rx_callback);
    furi_hal_usb_set_config(&midi_usb_interface, NULL);

    uint32_t events;
    while(1) {
        events = furi_thread_flags_wait(MidiThreadEventAll, FuriFlagWaitAny, FuriWaitForever);

        if(!(events & FuriFlagError)) {
            if(events & MidiThreadEventRx) {
                uint8_t buffer[64];
                size_t size = midi_usb_rx(buffer, sizeof(buffer));
                midi_usb_tx(buffer, size);

                FuriString* string = furi_string_alloc();
                furi_string_printf(string, "MIDI RX: ");
                for(size_t i = 0; i < size; i++) {
                    furi_string_cat_printf(string, "%02X ", buffer[i]);
                }
                FURI_LOG_I("USB_MIDI", "%s", furi_string_get_cstr(string));
            }
        }
    }

    furi_hal_usb_set_config(usb_config_prev, NULL);

    return 0;
}