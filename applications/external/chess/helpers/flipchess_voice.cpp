#include "flipchess_voice.h"
#include <furi.h>
#include <furi_hal.h>
#include "../sam/stm32_sam.h"
STM32SAM voice;

void flipchess_voice_shall_we_play() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("SHAAL WE PLAY AY GAME?");
        furi_hal_speaker_release();
    }
}

void flipchess_voice_which_side() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("WHICH SIDE DO YOU WANT?");
        furi_hal_speaker_release();
    }
}

void flipchess_voice_how_about_chess() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("HOW ABOUT A NICE GAME OF CHESS?");
        furi_hal_speaker_release();
    }
}

void flipchess_voice_a_strange_game() {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say("A STRANGE GAME... THE ONLY WINNING MOVE IS NOT TO PLAY.");
        furi_hal_speaker_release();
    }
}