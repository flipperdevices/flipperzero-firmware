/**
 * @file furi_hal_speaker.h
 * Speaker HAL
 */
#pragma once

#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Init speaker */
void furi_hal_speaker_init();

/** Deinit speaker */
void furi_hal_speaker_deinit();

/** Acquire speaker 
 * Speaker control is only given to 1 process at a time
 * 
 * @param  timeout Timeout during which acquire speaker
 * @return   bool  returns true if the attempt was successful
 */
bool __attribute__((warn_unused_result)) furi_hal_speaker_acquire(uint32_t timeout);

/** Release speaker */
void furi_hal_speaker_release();

/** Ñheck that the current process owns the speaker
 * 
 * @return   bool
 */
bool furi_hal_speaker_is_mine();

/** Play a note
 * 
 * @param  frequency 
 * @param  volume 
 */
void furi_hal_speaker_start(float frequency, float volume);

/** Set volume
 * 
 * @param  volume 
 */
void furi_hal_speaker_set_volume(float volume);

/** Stop playback  */
void furi_hal_speaker_stop();

#ifdef __cplusplus
}
#endif
