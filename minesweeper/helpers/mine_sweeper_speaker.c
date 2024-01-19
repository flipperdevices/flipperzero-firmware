#include "mine_sweeper_speaker.h"
#include "../minesweeper.h"

static const float volume = 0.8f;

void mine_sweeper_play_ok_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_LOSE, volume);
    }
}

void mine_sweeper_play_flag_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_FLAG, volume);
    }
}

void mine_sweeper_play_oob_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_OOB, volume);
    }
}

void mine_sweeper_play_win_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_WIN, volume);
    }
}

void mine_sweeper_play_lose_sound(void* context) {
    MineSweeperApp* app = context;
    UNUSED(app);

    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_LOSE, volume);
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
