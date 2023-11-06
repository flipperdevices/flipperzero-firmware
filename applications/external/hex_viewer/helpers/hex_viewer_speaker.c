#include "hex_viewer_speaker.h"
#include "../hex_viewer.h"

#define NOTE_INPUT 587.33f

void hex_viewer_play_input_sound(void* context) {
    HexViewer* app = context;
    if(app->speaker != 1) {
        return;
    }
    float volume = 1.0f;
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_INPUT, volume);
    }
}

void hex_viewer_stop_all_sound(void* context) {
    HexViewer* app = context;
    if(app->speaker != 1) {
        return;
    }
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}
