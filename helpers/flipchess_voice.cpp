#include "flipchess_voice.h"
#include <furi.h>
#include <furi_hal.h>
#include "../sam/stm32_sam.h"
STM32SAM voice;

int32_t flipchess_voice_game(void* p) {
    UNUSED(p);
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("SHAAL WE PLAY AY GAME?");
        furi_hal_speaker_release();
    }
    return 0;
}

int32_t flipchess_voice_no(void* p) {
    UNUSED(p);
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("No");
        furi_hal_speaker_release();
    }
    return 0;
}