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
 * @warning Speaker control is only given to 1 process at a time
 * 
 * @param  timeout Timeout during which acquire speaker
 * @return   bool  returns true if the attempt was successful
 */
bool __attribute__((warn_unused_result)) furi_hal_speaker_acquire(uint32_t timeout);

/** Release speaker 
 * 
 * @warning No check is made that the current thread owns the process when called from an interrupt
*/
void furi_hal_speaker_release();

/** Ñheck that the current process owns the speaker
 * 
 * @warning Returns always True when called from an interrupt
 * @return   bool
 */
bool furi_hal_speaker_is_mine();

/** Play a note
 * 
 * @warning No check is made that the current thread owns the process when called from an interrupt
 * @param  frequency 
 * @param  volume 
 */
void furi_hal_speaker_start(float frequency, float volume);

/** Set volume
 * 
 * @warning No check is made that the current thread owns the process when called from an interrupt
 * @param  volume 
 */
void furi_hal_speaker_set_volume(float volume);

/** Stop playback  
 * 
 * @warning No check is made that the current thread owns the process when called from an interrupt
*/
void furi_hal_speaker_stop();

#ifdef __cplusplus
}
#endif
