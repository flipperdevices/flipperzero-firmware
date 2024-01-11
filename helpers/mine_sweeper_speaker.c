#include "mine_sweeper_speaker.h"
#include "../minesweeper.h"

void mine_sweeper_play_flag_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    float volume = 0.8f;
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_INPUT, volume);
    }
    
}

void mine_sweeper_stop_all_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}
