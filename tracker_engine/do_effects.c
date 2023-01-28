#include "do_effects.h"
#include <furi.h>

#include "../sound_engine/sound_engine_filter.h"

void do_command(uint16_t opcode, TrackerEngine *tracker_engine, uint8_t channel, uint8_t tick, bool from_program)
{
    UNUSED(from_program);

    TrackerEngineChannel *te_channel = &tracker_engine->channel[channel];
    SoundEngineChannel *se_channel = &tracker_engine->sound_engine->channel[channel];

    switch (opcode & 0x7f00)
    {
        case TE_EFFECT_ARPEGGIO:
        {
            if (tick == 0)
            {
                if (te_channel->fixed_note != 0xffff)
                {
                    te_channel->note = te_channel->last_note;
                    te_channel->fixed_note = 0xffff;
                }

                if ((opcode & 0xff) == 0xf0)
                    te_channel->arpeggio_note = te_channel->extarp1;
                else if ((opcode & 0xff) == 0xf1)
                    te_channel->arpeggio_note = te_channel->extarp2;
                else
                    te_channel->arpeggio_note = (opcode & 0xff);
            }
            break;
        }

        case TE_EFFECT_PORTAMENTO_UP:
        {
            uint32_t prev = te_channel->note;

            te_channel->note += ((opcode & 0xff) << 2);
            if (prev > te_channel->note)
                te_channel->note = 0xffff;

            te_channel->target_note = te_channel->note;
            break;
        }

        case TE_EFFECT_PORTAMENTO_DOWN:
        {
            int32_t prev = te_channel->note;

            te_channel->note -= ((opcode & 0xff) << 2);
            if (prev < te_channel->note)
                te_channel->note = 0;

            te_channel->target_note = te_channel->note;
            break;
        }

        case TE_EFFECT_VIBRATO:
        {
            if (tick == 0)
            {
                if (opcode & 0xff)
                {
                    te_channel->flags |= TE_ENABLE_VIBRATO;

                    te_channel->vibrato_speed = (opcode & 0xf0);
                    te_channel->vibrato_depth = ((opcode & 0x0f) << 4);
                }

                else
                {
                    te_channel->flags &= ~(TE_ENABLE_VIBRATO);
                }
            }

            break;
        }

        case TE_EFFECT_PWM:
        {
            if (tick == 0)
            {
                if (opcode & 0xff)
                {
                    te_channel->flags |= TE_ENABLE_PWM;

                    te_channel->pwm_speed = (opcode & 0xf0);
                    te_channel->pwm_depth = ((opcode & 0x0f) << 4);
                }

                else
                {
                    te_channel->flags &= ~(TE_ENABLE_PWM);
                }
            }

            break;
        }

        case TE_EFFECT_SET_PW:
        {
            if (tick == 0)
            {
                te_channel->pw = ((opcode & 0xff) << 4);
            }

            break;
        }

        case TE_EFFECT_PW_UP:
        {
            int16_t temp_pw = te_channel->pw + (int16_t)(opcode & 0xff);

            if (temp_pw < 0)
                temp_pw = 0;
            if (temp_pw > 0xfff)
                temp_pw = 0xfff;

            te_channel->pw = temp_pw;

            break;
        }

        case TE_EFFECT_PW_DOWN:
        {
            int16_t temp_pw = te_channel->pw - (int16_t)(opcode & 0xff);

            if (temp_pw < 0)
                temp_pw = 0;
            if (temp_pw > 0xfff)
                temp_pw = 0xfff;

            te_channel->pw = temp_pw;

            break;
        }

        case TE_EFFECT_SET_CUTOFF:
        {
            if (tick == 0)
            {
                te_channel->filter_cutoff = ((opcode & 0xff) << 3);
                sound_engine_filter_set_coeff(&se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
            }

            break;
        }

        case TE_EFFECT_VOLUME_FADE:
        {
            if (!(te_channel->channel_flags & TEC_DISABLED))
            {
                te_channel->volume -= opcode & 0xf;
                if (te_channel->volume > MAX_ADSR_VOLUME)
                    te_channel->volume = 0;
                te_channel->volume += (opcode >> 4) & 0xf;
                if (te_channel->volume > MAX_ADSR_VOLUME)
                    te_channel->volume = MAX_ADSR_VOLUME;

                se_channel->adsr.volume = (int32_t)se_channel->adsr.volume * (int32_t)te_channel->volume / MAX_ADSR_VOLUME * (int32_t)te_channel->instrument->adsr.volume / MAX_ADSR_VOLUME;
                se_channel->adsr.volume = (int32_t)se_channel->adsr.volume * (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
            }

            break;
        }

        case TE_EFFECT_SET_WAVEFORM:
        {
            if (tick == 0)
            {
                se_channel->waveform = (opcode & 0x3f);
            }

            break;
        }

        case TE_EFFECT_SET_VOLUME:
        {
            if (tick == 0)
            {
                if (!(te_channel->channel_flags & TEC_DISABLED))
                {
                    te_channel->volume = opcode & 0xff;

                    se_channel->adsr.volume = (int32_t)se_channel->adsr.volume * (int32_t)te_channel->volume / MAX_ADSR_VOLUME * (int32_t)te_channel->instrument->adsr.volume / MAX_ADSR_VOLUME;
                    se_channel->adsr.volume = (int32_t)se_channel->adsr.volume * (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
                }
            }

            break;
        }

        case TE_EFFECT_SET_SPEED_PROG_PERIOD:
        {
            if (tick == 0)
            {
                if (from_program)
                {
                    te_channel->program_period = opcode & 0xff;
                }

                else
                {
                    tracker_engine->song->speed = opcode & 0xff;
                }
            }

            break;
        }

        default:
            break;
    }
}