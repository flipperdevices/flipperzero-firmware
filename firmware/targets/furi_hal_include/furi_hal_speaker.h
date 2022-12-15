/**
 * @file furi_hal_speaker.h
 * Speaker HAL
 */
#pragma once

#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_hal_speaker_init();

void furi_hal_speaker_deinit();

bool __attribute__((warn_unused_result)) furi_hal_speaker_acquire(uint32_t timeout);

void furi_hal_speaker_release();

bool furi_hal_speaker_is_mine();

void furi_hal_speaker_start(float frequency, float volume);

void furi_hal_speaker_set_volume(float volume);

void furi_hal_speaker_stop();

#ifdef __cplusplus
}
#endif
