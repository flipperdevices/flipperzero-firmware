#include "audio_helper.h"

bool initializeSpeaker() {
    return furi_hal_speaker_acquire(SPEAKER_TIMEOUT);
}

bool startSound(struct ToneData_t* toneData) {
    bool hasSpeaker = furi_hal_speaker_is_mine();
    if(!hasSpeaker) {
        hasSpeaker = furi_hal_speaker_acquire(SPEAKER_TIMEOUT);
    }

    if(hasSpeaker) {
        FURI_LOG_I(TAG, "Sound is beginning to play");
        furi_hal_speaker_start(toneData->frequency, toneData->volume);
    } else {
        FURI_LOG_E(TAG, "Error acquiring speaker!");
    }
    return hasSpeaker;
}

void stopSound() {
    furi_hal_speaker_stop();
}

void deinitializeSpeaker() {
    furi_hal_speaker_release();
}
