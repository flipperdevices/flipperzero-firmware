#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_serial.h>
#include "midi/parser.h"
#include "midi/message.h"
#include <math.h>
#include <gui/gui.h>
#include <input/input.h>
#include "midi_rx_icons.h"

#define MIDI_UART_BUFFER_SIZE 256
#define MAX_SIMULTANEOUS_NOTES 10

float note_to_frequency(int note) {
    float a = 440;
    return (a / 32) * powf(2, ((note - 9) / 12.0));
}

typedef enum {
    MidiRxThreadEventStop = (1 << 0),
    MidiRxThreadEventRxDone = (1 << 1),
} MidiRxThreadEvent;

typedef struct {
    FuriThread* rx_thread;
    FuriStreamBuffer* rx_stream;
    uint8_t rx_buf[MIDI_UART_BUFFER_SIZE];
    FuriHalSerialHandle* serial_handle;
    uint8_t active_notes[MAX_SIMULTANEOUS_NOTES];
    size_t active_notes_count;
    Gui* gui;
    ViewPort* view_port;
    bool should_exit;
} MidiUart;

static void
    midi_rx_callback(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    MidiUart* midi_uart = (MidiUart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(midi_uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(midi_uart->rx_thread), MidiRxThreadEventRxDone);
    }
}

static void midi_uart_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "MIDI Receiver");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 18, "Attach MIDI Module");
    canvas_draw_str(canvas, 2, 41, "TTL232's TX");
    canvas_draw_str(canvas, 2, 50, "Flipper's RX");
    canvas_draw_str(canvas, 2, 60, "and both GND");
    canvas_draw_icon(canvas, 72, 16, &I_DolphinCommon_56x48);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 38, 29, "or");
}

static void midi_uart_input_callback(InputEvent* input_event, void* ctx) {
    MidiUart* midi_uart = (MidiUart*)ctx;
    if(input_event->type == InputTypeShort && input_event->key == InputKeyBack) {
        if(furi_hal_speaker_is_mine()) {
            furi_hal_speaker_stop();
            furi_hal_speaker_release();
        }
        furi_hal_serial_async_rx_stop(midi_uart->serial_handle);
        midi_uart->should_exit = true;
    }
}

static int32_t midi_rx_thread(void* context) {
    MidiUart* midi_uart = (MidiUart*)context;
    MidiParser* parser = midi_parser_alloc();

    while(1) {
        uint32_t events = furi_thread_flags_wait(
            MidiRxThreadEventStop | MidiRxThreadEventRxDone, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);
        if(events & MidiRxThreadEventStop) break;
        if(events & MidiRxThreadEventRxDone) {
            size_t len = furi_stream_buffer_receive(
                midi_uart->rx_stream, midi_uart->rx_buf, MIDI_UART_BUFFER_SIZE, 0);
            if(len > 0) {
                for(size_t i = 0; i < len; i++) {
                    if(midi_parser_parse(parser, midi_uart->rx_buf[i])) {
                        MidiEvent* event = midi_parser_get_message(parser);
                        if(event->type == NoteOn) {
                            NoteOnEvent note_on = AsNoteOn(event);
                            if(note_on.velocity > 0) {
                                if(midi_uart->active_notes_count < MAX_SIMULTANEOUS_NOTES) {
                                    midi_uart->active_notes[midi_uart->active_notes_count++] =
                                        note_on.note;
                                    if(furi_hal_speaker_is_mine() ||
                                       furi_hal_speaker_acquire(1000)) {
                                        furi_hal_speaker_start(
                                            note_to_frequency(note_on.note),
                                            note_on.velocity / 127.0f);
                                    }
                                }
                            } else {
                                for(size_t j = 0; j < midi_uart->active_notes_count; j++) {
                                    if(midi_uart->active_notes[j] == note_on.note) {
                                        for(size_t k = j; k < midi_uart->active_notes_count - 1;
                                            k++) {
                                            midi_uart->active_notes[k] =
                                                midi_uart->active_notes[k + 1];
                                        }
                                        midi_uart->active_notes_count--;
                                        break;
                                    }
                                }
                                if(midi_uart->active_notes_count == 0) {
                                    if(furi_hal_speaker_is_mine()) {
                                        furi_hal_speaker_stop();
                                        furi_hal_speaker_release();
                                    }
                                }
                            }
                        } else if(event->type == NoteOff) {
                            NoteOffEvent note_off = AsNoteOff(event);
                            for(size_t j = 0; j < midi_uart->active_notes_count; j++) {
                                if(midi_uart->active_notes[j] == note_off.note) {
                                    for(size_t k = j; k < midi_uart->active_notes_count - 1; k++) {
                                        midi_uart->active_notes[k] =
                                            midi_uart->active_notes[k + 1];
                                    }
                                    midi_uart->active_notes_count--;
                                    break;
                                }
                            }
                            if(midi_uart->active_notes_count == 0) {
                                if(furi_hal_speaker_is_mine()) {
                                    furi_hal_speaker_stop();
                                    furi_hal_speaker_release();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    midi_parser_free(parser);
    furi_stream_buffer_free(midi_uart->rx_stream);

    return 0;
}

int32_t midi_rx_app(void* p) {
    UNUSED(p);

    MidiUart* midi_uart = malloc(sizeof(MidiUart));
    midi_uart->serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_check(midi_uart->serial_handle);
    midi_uart->rx_stream = furi_stream_buffer_alloc(MIDI_UART_BUFFER_SIZE, 1);
    midi_uart->rx_thread = furi_thread_alloc();
    furi_thread_set_name(midi_uart->rx_thread, "MidiRxThread");
    furi_thread_set_stack_size(midi_uart->rx_thread, 1024);
    furi_thread_set_context(midi_uart->rx_thread, midi_uart);
    furi_thread_set_callback(midi_uart->rx_thread, midi_rx_thread);
    midi_uart->active_notes_count = 0;
    midi_uart->should_exit = false;
    furi_thread_start(midi_uart->rx_thread);

    midi_uart->gui = furi_record_open(RECORD_GUI);
    midi_uart->view_port = view_port_alloc();
    view_port_draw_callback_set(midi_uart->view_port, midi_uart_draw_callback, midi_uart);
    view_port_input_callback_set(midi_uart->view_port, midi_uart_input_callback, midi_uart);
    gui_add_view_port(midi_uart->gui, midi_uart->view_port, GuiLayerFullscreen);

    furi_hal_serial_init(midi_uart->serial_handle, 31250);
    furi_hal_serial_enable_direction(midi_uart->serial_handle, FuriHalSerialDirectionRx);
    furi_hal_serial_async_rx_start(midi_uart->serial_handle, midi_rx_callback, midi_uart, true);

    while(!midi_uart->should_exit) {
        furi_delay_ms(1);
    }

    furi_delay_ms(100);

    furi_hal_serial_async_rx_stop(midi_uart->serial_handle);
    furi_hal_serial_disable_direction(midi_uart->serial_handle, FuriHalSerialDirectionRx);
    furi_hal_serial_deinit(midi_uart->serial_handle);
    furi_hal_serial_control_release(midi_uart->serial_handle);

    view_port_enabled_set(midi_uart->view_port, false);
    gui_remove_view_port(midi_uart->gui, midi_uart->view_port);
    view_port_free(midi_uart->view_port);
    furi_record_close(RECORD_GUI);

    furi_thread_flags_set(furi_thread_get_id(midi_uart->rx_thread), MidiRxThreadEventStop);
    furi_thread_join(midi_uart->rx_thread);
    furi_thread_free(midi_uart->rx_thread);

    free(midi_uart);

    return 0;
}