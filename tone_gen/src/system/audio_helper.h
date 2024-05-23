#ifndef _AUDIO_HELPER_H_

#define _AUDIO_HELPER_H_

#include <furi_hal.h>

#include "../tone_gen.h"

#define SPEAKER_TIMEOUT 10

bool initializeSpeaker();
bool startSound(struct ToneData_t* toneData);
void stopSound();
void deinitializeSpeaker();

#endif
