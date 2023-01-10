#include "sound_engine.h"
#include "../flizzer_tracker_hal.h"

#include <furi_hal.h>

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

	sound_engine_init_hardware(sample_rate, external_audio_output, sound_engine->audio_buffer, audio_buffer_size);
}

void sound_engine_deinit(SoundEngine* sound_engine)
{
	free(sound_engine->audio_buffer);

	if(!(sound_engine->external_audio_output))
	{
		furi_hal_speaker_release();
	}
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

static inline uint16_t sound_engine_pulse(uint32_t acc, uint32_t pw) //0-FFF pulse width range
{
	return (((acc >> (((uint32_t)ACC_BITS - 17))) >= ((pw == 0xfff ? pw + 1 : pw) << 4) ? (WAVE_AMP - 1) : 0));
}


static inline uint16_t sound_engine_saw(uint32_t acc) 
{
	return (acc >> (ACC_BITS - OUTPUT_BITS - 1)) & (WAVE_AMP - 1);
}


static inline uint16_t sound_engine_triangle(uint32_t acc)
{
	return ((((acc & (ACC_LENGTH / 2)) ? ~acc : acc) >> (ACC_BITS - OUTPUT_BITS - 2)) & (WAVE_AMP * 2 - 1));
}

inline static void shift_lfsr(uint32_t* v, uint32_t tap_0, uint32_t tap_1)
{
	typedef uint32_t T;
	const T zero = (T)(0);
	const T lsb = zero + (T)(1);
	const T feedback = (
		(lsb << (tap_0)) ^
		(lsb << (tap_1))
	);

	*v = (*v >> 1) ^ ((zero - (*v & lsb)) & feedback);
}

uint16_t sound_engine_osc(SoundEngineChannel* channel, uint32_t prev_acc)
{
	switch(channel->waveform)
	{
		case SE_WAVEFORM_NOISE:
		{
			//return sound_engine_noise(channel->accumulator, prev_acc, &channel->lfsr);
			if((prev_acc & (ACC_LENGTH / 32)) != (channel->accumulator & (ACC_LENGTH / 32)))
			{
				shift_lfsr(&channel->lfsr, 22, 17);
				channel->lfsr &= (1 << (22 + 1)) - 1;
			}

			return (channel->lfsr) & (WAVE_AMP - 1);

			break;
		}

		case SE_WAVEFORM_PULSE:
		{
			return sound_engine_pulse(channel->accumulator, channel->pw);
			break;
		}

		case SE_WAVEFORM_TRIANGLE:
		{
			return sound_engine_triangle(channel->accumulator);
			break;
		}
		
		case SE_WAVEFORM_SAW:
		{
			return sound_engine_saw(channel->accumulator);
			break;
		}
	}

	return 0;
}

void sound_engine_fill_buffer(SoundEngine* sound_engine, uint16_t* audio_buffer, uint32_t audio_buffer_size)
{
	for(uint32_t i = 0; i < audio_buffer_size; ++i)
	{
		uint16_t output = 0;

		for(uint32_t chan = 0; chan < NUM_CHANNELS; ++chan)
		{
			SoundEngineChannel* channel = &sound_engine->channel[chan];

			if(channel->frequency > 0)
			{
				uint32_t prev_acc = channel->accumulator;

				channel->accumulator += channel->frequency;
				channel->accumulator &= ACC_LENGTH - 1;

				output += (sound_engine_osc(channel, prev_acc) >> (6 + 2));
			}
		}

		audio_buffer[i] = output; //2 more bits so all channels fit
	}
}