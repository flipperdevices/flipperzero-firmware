// this code was tested in Flipper Zero SDK v50.1 to v54.0

//It is still under development.
//I would like to add various functions when we feel like it.
//It's unstable sometimes, so please tolerate it.
//The license for this FAP shall be WTFPL.

#include <furi.h>
#include <string.h>
#include <furi_hal.h>
#include "usb/usb_midi_driver.h"
#include "midi/message.h"
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>

#define MUSIC_PLAYER_APP_EXTENSION "*"
#define TAG "FMF2USB_MIDI"
#define MUSIC_PLAYER_APP_PATH_FOLDER "/ext/music_player"

typedef enum { FmStatus_Idle, FmStatus_Loading, FmStatus_Sending } FmStatus;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FmStatus status;
    bool should_exit;
    uint8_t beat;
} Fmf2UsbMidiApp;

bool convert_fmf_to_midi(
    const char* file_content,
    uint8_t* midi_data,
    size_t* midi_data_size,
    int* bpm);

void fmf2usb_midi_play_file(const char* file_path, Fmf2UsbMidiApp* app);
void fmf2usb_midi_render_callback(Canvas* canvas, void* ctx);
void fmf2usb_midi_input_callback(InputEvent* input_event, void* ctx);

void fmf2usb_midi_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    Fmf2UsbMidiApp* app = ctx;

    if(input_event->key == InputKeyBack && input_event->type == InputTypeShort) {
        app->should_exit = true;
    }
}

Fmf2UsbMidiApp* fmf2usb_midi_app_alloc() {
    Fmf2UsbMidiApp* app = malloc(sizeof(Fmf2UsbMidiApp));
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, fmf2usb_midi_render_callback, app);
    view_port_input_callback_set(app->view_port, fmf2usb_midi_input_callback, app);
    app->status = FmStatus_Idle;
    app->should_exit = false;
    return app;
}

void fmf2usb_midi_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    Fmf2UsbMidiApp* app = ctx;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(app->status) {
    case FmStatus_Idle:
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "Select an FMF file");
        break;
    case FmStatus_Loading:
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, "Hold on...");
        break;
    case FmStatus_Sending:
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "Sending MIDI via USB");
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignCenter, "Press Back to exit");
        break;
    }
}

bool convert_fmf_to_midi(
    const char* file_content,
    uint8_t* midi_data,
    size_t* midi_data_size,
    int* bpm) {
    FURI_LOG_D(TAG, "Converting FMF to MIDI...");
    // analyze the fmf file
    char* lines = strdup(file_content);
    char* line = strtok(lines, "\n");

    int duration = 0;
    int octave = 0;

    while(line != NULL) {
        if(strncmp(line, "BPM:", 4) == 0) {
            sscanf(line, "BPM: %d", bpm);
        } else if(strncmp(line, "Duration:", 9) == 0) {
            sscanf(line, "Duration: %d", &duration);
        } else if(strncmp(line, "Octave:", 7) == 0) {
            sscanf(line, "Octave: %d", &octave);
        } else if(strncmp(line, "Notes:", 6) == 0) {
            char* notes = line + 7;
            char* note = strtok(notes, ", ");

            while(note != NULL) {
                int note_duration = duration;
                int note_octave = octave;
                int dots = 0;
                bool is_sharp = false;

                // note duration analyze
                if(isdigit((unsigned char)*note)) {
                    char* duration_str = note;
                    while(isdigit((unsigned char)*duration_str)) {
                        duration_str++;
                    }
                    if(duration_str > note) {
                        note_duration = atoi(note);
                        note = duration_str;
                    }
                } else {
                    note_duration = duration;
                }

                // note name
                char note_name = *note++;

                // sharp
                if(*note == '#') {
                    is_sharp = true;
                    note++;
                }

                // octave
                if(isdigit((unsigned char)*note)) {
                    note_octave = *note++ - '0';
                }

                // dots
                while(*note == '.') {
                    dots++;
                    note++;
                }

                int note_delay = (60000 / (*bpm * 2)) * (duration / note_duration);
                int total_delay = note_delay;
                for(int i = 0; i < dots; i++) {
                    total_delay += note_delay / (1 << (i + 1));
                }

                if(note_name == 'P') {
                    // when P is used, insert a delay
                    midi_data[(*midi_data_size)++] = 0x00; // Delay
                    midi_data[(*midi_data_size)++] = (total_delay >> 8) & 0xFF;
                    midi_data[(*midi_data_size)++] = total_delay & 0xFF;
                } else {
                    int note_number = 0;

                    switch(note_name) {
                    case 'C':
                        note_number = 0;
                        break;
                    case 'D':
                        note_number = 2;
                        break;
                    case 'E':
                        note_number = 4;
                        break;
                    case 'F':
                        note_number = 5;
                        break;
                    case 'G':
                        note_number = 7;
                        break;
                    case 'A':
                        note_number = 9;
                        break;
                    case 'B':
                        note_number = 11;
                        break;
                    }

                    if(is_sharp) {
                        note_number++;
                    }

                    note_number += (note_octave * 12);

                    midi_data[(*midi_data_size)++] = 0x90; // Note On
                    midi_data[(*midi_data_size)++] = note_number;
                    midi_data[(*midi_data_size)++] = 100; // Velocity

                    int note_on_delay = total_delay * 9 / 10;
                    midi_data[(*midi_data_size)++] = 0x00; // Delay
                    midi_data[(*midi_data_size)++] = (note_on_delay >> 8) & 0xFF;
                    midi_data[(*midi_data_size)++] = note_on_delay & 0xFF;

                    midi_data[(*midi_data_size)++] = 0x80; // Note Off
                    midi_data[(*midi_data_size)++] = note_number;
                    midi_data[(*midi_data_size)++] = 0; // Velocity

                    int note_off_delay = total_delay - note_on_delay;
                    midi_data[(*midi_data_size)++] = 0x00; // Delay
                    midi_data[(*midi_data_size)++] = (note_off_delay >> 8) & 0xFF;
                    midi_data[(*midi_data_size)++] = note_off_delay & 0xFF;
                }

                note = strtok(NULL, ", ");
            }
        }

        line = strtok(NULL, "\n");
    }

    free(lines);
    FURI_LOG_D(TAG, "Conversion complete. MIDI data size: %d", *midi_data_size);
    return true;
}

void fmf2usb_midi_play_file(const char* file_path, Fmf2UsbMidiApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    if(!storage_file_open(file, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        return;
    }

    char* file_content = malloc(storage_file_size(file) + 1);
    storage_file_read(file, file_content, storage_file_size(file));
    file_content[storage_file_size(file)] = '\0';

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    uint8_t* midi_data = malloc(1024);
    size_t midi_data_size = 0;
    int bpm = 0;

    if(convert_fmf_to_midi(file_content, midi_data, &midi_data_size, &bpm)) {
        FURI_LOG_D(TAG, "Playing MIDI data...");

        app->status = FmStatus_Sending;
        app->beat = 0;
        view_port_update(app->view_port);

        size_t pos = 0;
        uint32_t led_update_time = furi_get_tick();

        while(pos < midi_data_size && !app->should_exit) {
            if(midi_data[pos] == 0x00) { // Delay
                uint32_t delay = (midi_data[pos + 1] << 8) | midi_data[pos + 2];
                if(delay > 0) {
                    FURI_LOG_D(TAG, "Delay: %ld ms", delay);
                    furi_delay_ms(delay);
                }
                pos += 3;
            } else {
                FURI_LOG_D(
                    TAG,
                    "Sending MIDI event: %02X %02X %02X",
                    midi_data[pos],
                    midi_data[pos + 1],
                    midi_data[pos + 2]);

                uint8_t midi_event[4];
                midi_event[0] = 0x09;
                midi_event[1] = midi_data[pos];
                midi_event[2] = midi_data[pos + 1];
                midi_event[3] = midi_data[pos + 2];
                midi_usb_tx(midi_event, 4);

                pos += 3;
            }
            uint32_t current_time = furi_get_tick();
            if(current_time - led_update_time >= (uint32_t)(60000 / bpm)) {
                led_update_time = current_time;
                app->beat = (app->beat + 1) % 4;

                if(app->beat == 0) {
                    furi_hal_light_set(LightGreen, 0xFF);
                    furi_hal_light_set(LightBlue, 0x00);
                } else {
                    furi_hal_light_set(LightGreen, 0x00);
                    furi_hal_light_set(LightBlue, 0xFF);
                }
            }
        }
        furi_hal_light_set(LightGreen, 0x00);
        furi_hal_light_set(LightBlue, 0x00);

        FURI_LOG_D(TAG, "Playback complete");
        FURI_LOG_D(TAG, "Playback complete");
    } else {
        FURI_LOG_E(TAG, "Failed to convert FMF to MIDI");
    }
    furi_delay_ms(500);

    free(midi_data);
    free(file_content);
}

void fmf2usb_midi_app_free(Fmf2UsbMidiApp* app) {
    furi_assert(app);
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    free(app);
}

int32_t fmf2usb_midi_app(void* p) {
    UNUSED(p);
    FURI_LOG_D(TAG, "Starting fmf2usb_midi_app");

    Fmf2UsbMidiApp* app = fmf2usb_midi_app_alloc();

    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&midi_usb_interface, NULL);
    furi_hal_usb_lock();

    FuriString* file_path;
    file_path = furi_string_alloc();
    furi_string_set(file_path, MUSIC_PLAYER_APP_PATH_FOLDER);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, MUSIC_PLAYER_APP_EXTENSION, NULL);
    browser_options.hide_ext = false;
    browser_options.base_path = MUSIC_PLAYER_APP_PATH_FOLDER;

    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    bool res = dialog_file_browser_show(dialogs, file_path, file_path, &browser_options);
    furi_record_close(RECORD_DIALOGS);

    if(res) {
        FURI_LOG_D(TAG, "File selected: %s", furi_string_get_cstr(file_path));
        app->status = FmStatus_Loading;
        view_port_update(app->view_port);
        fmf2usb_midi_play_file(furi_string_get_cstr(file_path), app);
    } else {
        FURI_LOG_D(TAG, "File selection cancelled");
    }

    furi_string_free(file_path);
    fmf2usb_midi_app_free(app);

    furi_hal_usb_unlock();
    furi_hal_usb_set_config(usb_mode_prev, NULL);
    FURI_LOG_D(TAG, "Exiting fmf2usb_midi_app");
    return 0;
}