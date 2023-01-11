#include "sound_engine.h"
#include "../flizzer_tracker.h"
#include "../flizzer_tracker_hal.h"

#include <furi_hal.h>

#define PI 3.1415

void sound_engine_init(SoundEngine* sound_engine, uint32_t sample_rate, bool external_audio_output, uint32_t audio_buffer_size)
{
	sound_engine->audio_buffer = malloc(audio_buffer_size * sizeof(sound_engine->audio_buffer[0]));
	sound_engine->audio_buffer_size = audio_buffer_size;
	sound_engine->sample_rate = sample_rate;
	sound_engine->external_audio_output = external_audio_output;

	for(int i = 0; i < NUM_CHANNELS; ++i)
	{
		sound_engine->channel[i].lfsr = RANDOM_SEED;
	}

	for(int i = 0; i < SINE_LUT_SIZE; ++i)
	{
		sound_engine->sine_lut[i] = (uint8_t)((sinf(i / 64.0 * PI) + 1.0) * 127.0);
	}

	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdDma1Ch1, 15, sound_engine_dma_isr, sound_engine);

	sound_engine_init_hardware(sample_rate, external_audio_output, sound_engine->audio_buffer, audio_buffer_size);
}

void sound_engine_deinit(SoundEngine* sound_engine)
{
	free(sound_engine->audio_buffer);

	if(!(sound_engine->external_audio_output))
	{
		furi_hal_speaker_release();
	}

	furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdDma1Ch1, 13, NULL, NULL);
	sound_engine_stop();
}

void sound_engine_set_channel_frequency(SoundEngine* sound_engine, SoundEngineChannel* channel, uint32_t frequency)
{
	if(frequency != 0)
	{
		channel->frequency = (uint64_t)(ACC_LENGTH) / (uint64_t)1024 * (uint64_t)(frequency) / (uint64_t)sound_engine->sample_rate;
	}

	else
	{
		channel->frequency = 0;
	}
}

void sound_engine_fill_buffer(SoundEngine* sound_engine, uint16_t* audio_buffer, uint32_t audio_buffer_size)
{
	for(uint32_t i = 0; i < audio_buffer_size; ++i)
	{
		int32_t output = WAVE_AMP / 2 / 64;

		for(uint32_t chan = 0; chan < NUM_CHANNELS; ++chan)
		{
			SoundEngineChannel* channel = &sound_engine->channel[chan];

			int32_t channel_output = 0;

			if(channel->frequency > 0)
			{
				uint32_t prev_acc = channel->accumulator;

				channel->accumulator += channel->frequency;
				channel->accumulator &= ACC_LENGTH - 1;

				channel_output += sound_engine_osc(sound_engine, channel, prev_acc) - WAVE_AMP / 2;

				if(channel->flags & SE_ENABLE_FILTER)
				{
					sound_engine_filter_cycle(&channel->filter, channel_output);

					switch(channel->filter_mode)
					{
						case FIL_OUTPUT_LOWPASS:
						{
							channel_output = sound_engine_output_lowpass(&channel->filter);
							break;
						}

						case FIL_OUTPUT_HIGHPASS:
						{
							channel_output = sound_engine_output_highpass(&channel->filter);
							break;
						}

						case FIL_OUTPUT_BANDPASS:
						{
							channel_output = sound_engine_output_bandpass(&channel->filter);
							break;
						}
					}
				}

				output += ((channel_output) / (64 * 4)); //2 more bits so all channels fit
			}
		}

		audio_buffer[i] = output;
	}
}