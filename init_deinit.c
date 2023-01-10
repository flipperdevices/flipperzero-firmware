#include "init_deinit.h"

void init_tracker(FlizzerTrackerApp* tracker, uint32_t sample_rate, bool external_audio_output, uint32_t audio_buffer_size)
{
	sound_engine_init(&tracker->sound_engine, sample_rate, external_audio_output, audio_buffer_size);
}

void deinit_tracker(FlizzerTrackerApp* tracker)
{
	sound_engine_deinit(&tracker->sound_engine);

	FURI_CRITICAL_ENTER();
	LL_TIM_DeInit(TIM1);
	LL_TIM_DeInit(TIM2);
	LL_TIM_DeInit(TIM16);
	FURI_CRITICAL_EXIT();
}