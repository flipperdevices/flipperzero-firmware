#include "evil_portal_speaker.h"
#include "../evil_portal_app_i.h"

#define NOTE_INPUT 587.33f

void evil_portal_play_input_sound() {
    float volume = 1.0f;
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_INPUT, volume);
    }
    
}

void evil_portal_stop_all_sound() {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}
