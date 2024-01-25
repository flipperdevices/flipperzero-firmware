#include "../flippertag.h"

void flippertag_play_sound(void* context, float frequency) {
    FlipperTag* app = context;
    if(app->speaker != 1) {
        return;
    }
    float volume = 1.0f;
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(frequency, volume);
    }
}

void flippertag_stop_all_sound(void* context) {
    FlipperTag* app = context;
    if(app->speaker != 1) {
        return;
    }
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}
