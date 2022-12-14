/**
 * @file furi_hal_speaker.h
 * Speaker HAL
 */
#pragma once

#include <furi_hal_speaker_config.h>
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_hal_speaker_init();

void furi_hal_speaker_deinit();

bool furi_hal_speaker_acquire(FuriHalSpeakerBusHandle* handle, uint32_t timeout);

void furi_hal_speaker_release(FuriHalSpeakerBusHandle* handle);

bool furi_hal_speaker_is_mine(FuriHalSpeakerBusHandle* handle);

void furi_hal_speaker_start(FuriHalSpeakerBusHandle* handle, float frequency, float volume);

void furi_hal_speaker_set_volume(FuriHalSpeakerBusHandle* handle, float volume);

void furi_hal_speaker_stop(FuriHalSpeakerBusHandle* handle);

#ifdef __cplusplus
}
#endif
