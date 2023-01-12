#include "init_deinit.h"

FlizzerTrackerApp* init_tracker(uint32_t sample_rate, uint8_t rate, bool external_audio_output, uint32_t audio_buffer_size)
{
	FlizzerTrackerApp* tracker = malloc(sizeof(FlizzerTrackerApp));
	memset(tracker, 0, sizeof(FlizzerTrackerApp));

	sound_engine_init(&tracker->sound_engine, sample_rate, external_audio_output, audio_buffer_size);
	tracker_engine_init(&tracker->tracker_engine, rate, &tracker->sound_engine);

	tracker->tracker_engine.song = &tracker->song;

	return tracker;
}

void deinit_tracker(FlizzerTrackerApp* tracker)
{
	sound_engine_deinit(&tracker->sound_engine);
	tracker_engine_deinit(&tracker->tracker_engine);

	FURI_CRITICAL_ENTER();
	LL_TIM_DeInit(TRACKER_ENGINE_TIMER);
	LL_TIM_DeInit(SAMPLE_RATE_TIMER);
	LL_TIM_DeInit(SPEAKER_PWM_TIMER);
	FURI_CRITICAL_EXIT();

	free(tracker);
}