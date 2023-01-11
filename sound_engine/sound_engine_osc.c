#include "sound_engine_osc.h"

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