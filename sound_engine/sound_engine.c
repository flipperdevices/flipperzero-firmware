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

static inline uint16_t sound_engine_sine(uint32_t acc, SoundEngine* sound_engine) 
{
	return (sound_engine->sine_lut[(acc >> (ACC_BITS - SINE_LUT_BITDEPTH))] << (OUTPUT_BITS - SINE_LUT_BITDEPTH));
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

uint16_t sound_engine_osc(SoundEngine* sound_engine, SoundEngineChannel* channel, uint32_t prev_acc)
{
	switch(channel->waveform)
	{
		case SE_WAVEFORM_NOISE:
		{
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

		case SE_WAVEFORM_NOISE_METAL:
		{
			if((prev_acc & (ACC_LENGTH / 32)) != (channel->accumulator & (ACC_LENGTH / 32)))
			{
				shift_lfsr(&channel->lfsr, 14, 8);
				channel->lfsr &= (1 << (14 + 1)) - 1;
			}

			return (channel->lfsr) & (WAVE_AMP - 1);

			break;
		}

		case SE_WAVEFORM_SINE:
		{
			return sound_engine_sine(channel->accumulator, sound_engine);
			break;
		}
	}

	return 0;
}

void sound_engine_filter_set_coeff(SoundEngineFilter *flt, uint32_t frequency, uint16_t resonance)
{
	flt->q = 2048 - frequency;
	flt->p = frequency + ((int32_t)(0.8f * 2048.0f) * frequency / 2048 * flt->q) / 2048;
	flt->f = flt->p + flt->p - 2048;
	flt->q = resonance;
}

void sound_engine_filter_cycle(SoundEngineFilter* flt, int32_t input)
{
	input -= flt->q * flt->b4 / 2048; //feedback
	int32_t t1 = flt->b1;  
	flt->b1 = (input + flt->b0) * flt->p / 2048 - flt->b1 * flt->f / 2048;
	int32_t t2 = flt->b2;  
	flt->b2 = (flt->b1 + t1) * flt->p / 2048 - flt->b2 * flt->f / 2048;
	t1 = flt->b3;
	flt->b3 = (flt->b2 + t2) * flt->p / 2048 - flt->b3 * flt->f / 2048;
	flt->b4 = (flt->b3 + t1) * flt->p / 2048 - flt->b4 * flt->f / 2048;
	
	//flt->b4 = my_min(32767, my_max(-32768, flt->b4));    //clipping
	
	flt->b0 = input;
}

int32_t sound_engine_output_lowpass(SoundEngineFilter* flt)
{
	return flt->b4;
}

int32_t sound_engine_output_highpass(SoundEngineFilter* flt)
{
	return flt->b0 - flt->b4;
}


int32_t sound_engine_output_bandpass(SoundEngineFilter* flt)
{
	return 3 * (flt->b3 - flt->b4);
}

void sound_engine_fill_buffer(SoundEngine* sound_engine, uint16_t* audio_buffer, uint32_t audio_buffer_size)
{
	for(uint32_t i = 0; i < audio_buffer_size; ++i)
	{
		int32_t output = 0;

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

				output += ((channel_output + WAVE_AMP / 2) >> (6 + 2)); //2 more bits so all channels fit
			}
		}

		audio_buffer[i] = output;
	}
}