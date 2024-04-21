#include "do_effects.h"
#include <furi.h>

#include "../sound_engine/sound_engine.h"
#include "../sound_engine/sound_engine_filter.h"
#include "tracker_engine.h"

#include "../flizzer_tracker_hal.h"

void do_command(
    uint16_t opcode,
    TrackerEngine* tracker_engine,
    uint8_t channel,
    uint8_t tick,
    bool from_program) {
    UNUSED(from_program);

    TrackerEngineChannel* te_channel = &tracker_engine->channel[channel];
    SoundEngineChannel* se_channel = &tracker_engine->sound_engine->channel[channel];

    switch(opcode & 0x7f00) {
    case TE_EFFECT_ARPEGGIO: {
        if(tick == 0) {
            if(te_channel->fixed_note != 0xffff) {
                te_channel->note = te_channel->last_note;
                te_channel->fixed_note = 0xffff;
            }

            if((opcode & 0xff) == 0xf0)
                te_channel->arpeggio_note = te_channel->extarp1;
            else if((opcode & 0xff) == 0xf1)
                te_channel->arpeggio_note = te_channel->extarp2;
            else
                te_channel->arpeggio_note = (opcode & 0xff);
        }
        break;
    }

    case TE_EFFECT_PORTAMENTO_UP: {
        uint32_t prev = te_channel->note;

        te_channel->note += ((opcode & 0xff) << 2);
        if(prev > te_channel->note) te_channel->note = 0xffff;

        te_channel->target_note = te_channel->note;
        break;
    }

    case TE_EFFECT_PORTAMENTO_DOWN: {
        int32_t prev = te_channel->note;

        te_channel->note -= ((opcode & 0xff) << 2);
        if(prev < te_channel->note) te_channel->note = 0;

        te_channel->target_note = te_channel->note;
        break;
    }

    case TE_EFFECT_VIBRATO: {
        if(tick == 0) {
            if(opcode & 0xff) {
                te_channel->flags |= TE_ENABLE_VIBRATO;

                te_channel->vibrato_speed = (opcode & 0xf0);
                te_channel->vibrato_depth = ((opcode & 0x0f) << 4);
            }

            else {
                te_channel->flags &= ~(TE_ENABLE_VIBRATO);
            }
        }

        break;
    }

    case TE_EFFECT_PWM: {
        if(tick == 0) {
            if(opcode & 0xff) {
                te_channel->flags |= TE_ENABLE_PWM;

                te_channel->pwm_speed = (opcode & 0xf0);
                te_channel->pwm_depth = ((opcode & 0x0f) << 4);
            }

            else {
                te_channel->flags &= ~(TE_ENABLE_PWM);
            }
        }

        break;
    }

    case TE_EFFECT_SET_PW: {
        if(tick == 0) {
            te_channel->pw = ((opcode & 0xff) << 4);
        }

        break;
    }

    case TE_EFFECT_PW_UP: {
        int16_t temp_pw = te_channel->pw + (int16_t)(opcode & 0xff);

        if(temp_pw < 0) temp_pw = 0;
        if(temp_pw > 0xfff) temp_pw = 0xfff;

        te_channel->pw = temp_pw;

        break;
    }

    case TE_EFFECT_PW_DOWN: {
        int16_t temp_pw = te_channel->pw - (int16_t)(opcode & 0xff);

        if(temp_pw < 0) temp_pw = 0;
        if(temp_pw > 0xfff) temp_pw = 0xfff;

        te_channel->pw = temp_pw;

        break;
    }

    case TE_EFFECT_SET_CUTOFF: {
        if(tick == 0) {
            te_channel->filter_cutoff = ((opcode & 0xff) << 3);
            sound_engine_filter_set_coeff(
                &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
        }

        break;
    }

    case TE_EFFECT_VOLUME_FADE: {
        if(!(te_channel->channel_flags & TEC_DISABLED)) {
            te_channel->volume -= (opcode & 0xf);
            if(te_channel->volume > MAX_ADSR_VOLUME) te_channel->volume = 0;
            te_channel->volume += ((opcode >> 4) & 0xf);
            if(te_channel->volume > MAX_ADSR_VOLUME) te_channel->volume = MAX_ADSR_VOLUME;

            se_channel->adsr.volume = (int32_t)te_channel->volume;
            se_channel->adsr.volume = (int32_t)se_channel->adsr.volume *
                                      (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
        }

        break;
    }

    case TE_EFFECT_SET_WAVEFORM: {
        if(tick == 0) {
            se_channel->waveform = (opcode & 0x3f);
        }

        break;
    }

    case TE_EFFECT_SET_VOLUME: {
        if(tick == 0) {
            if(!(te_channel->channel_flags & TEC_DISABLED)) {
                te_channel->volume = opcode & 0xff;

                se_channel->adsr.volume = (int32_t)te_channel->volume;
                se_channel->adsr.volume = (int32_t)se_channel->adsr.volume *
                                          (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
            }
        }

        break;
    }

    case TE_EFFECT_EXT: {
        switch(opcode & 0x7ff0) {
        case TE_EFFECT_EXT_TOGGLE_FILTER: {
            if(tick == 0) {
                if(opcode & 0xf) {
                    se_channel->flags |= SE_ENABLE_FILTER;
                }

                else {
                    se_channel->flags &= ~SE_ENABLE_FILTER;
                }
            }

            break;
        }

        case TE_EFFECT_EXT_PORTA_DN: {
            if(tick == 0) {
                int32_t prev = te_channel->note;

                te_channel->note -= (opcode & 0xf);
                if(prev < te_channel->note) te_channel->note = 0;

                te_channel->target_note = te_channel->note;
            }

            break;
        }

        case TE_EFFECT_EXT_PORTA_UP: {
            if(tick == 0) {
                uint32_t prev = te_channel->note;

                te_channel->note += (opcode & 0xf);
                if(prev > te_channel->note) te_channel->note = 0xffff;

                te_channel->target_note = te_channel->note;
            }

            break;
        }

        case TE_EFFECT_EXT_FILTER_MODE: {
            if(tick == 0) {
                se_channel->filter_mode = (opcode & 0xf);
            }

            break;
        }

        case TE_EFFECT_EXT_RETRIGGER: {
            if((opcode & 0xf) > 0 && (tick % (opcode & 0xf)) == 0) {
                uint8_t prev_vol_tr = te_channel->volume;
                uint8_t prev_vol_cyd = se_channel->adsr.volume;
                tracker_engine_trigger_instrument_internal(
                    tracker_engine, channel, te_channel->instrument, te_channel->last_note);
                te_channel->volume = prev_vol_tr;
                se_channel->adsr.volume = prev_vol_cyd;
            }

            break;
        }

        case TE_EFFECT_EXT_FINE_VOLUME_DOWN: {
            if(tick == 0) {
                te_channel->volume -= (opcode & 0xf);

                if(te_channel->volume > MAX_ADSR_VOLUME) te_channel->volume = 0;

                se_channel->adsr.volume = (int32_t)te_channel->volume;
                se_channel->adsr.volume = (int32_t)se_channel->adsr.volume *
                                          (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
            }

            break;
        }

        case TE_EFFECT_EXT_FINE_VOLUME_UP: {
            if(tick == 0) {
                te_channel->volume += (opcode & 0xf);

                if(te_channel->volume > MAX_ADSR_VOLUME) te_channel->volume = MAX_ADSR_VOLUME;

                se_channel->adsr.volume = (int32_t)te_channel->volume;
                se_channel->adsr.volume = (int32_t)se_channel->adsr.volume *
                                          (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
            }

            break;
        }

        case TE_EFFECT_EXT_NOTE_CUT: {
            if((opcode & 0xf) <= tick) {
                se_channel->adsr.volume = 0;
                te_channel->volume = 0;
            }

            break;
        }

        case TE_EFFECT_EXT_PHASE_RESET: {
            if(tick == (opcode & 0xf)) {
                se_channel->accumulator = 0;
                se_channel->lfsr = RANDOM_SEED;
            }

            break;
        }
        }

        break;
    }

    case TE_EFFECT_SET_SPEED_PROG_PERIOD: {
        if(tick == 0) {
            if(from_program) {
                te_channel->program_period = opcode & 0xff;
            }

            else {
                tracker_engine->song->speed = opcode & 0xff;
            }
        }

        break;
    }

    case TE_EFFECT_CUTOFF_UP: {
        te_channel->filter_cutoff += (opcode & 0xff);

        if(te_channel->filter_cutoff > 0x7ff) {
            te_channel->filter_cutoff = 0x7ff;
        }

        sound_engine_filter_set_coeff(
            &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);

        break;
    }

    case TE_EFFECT_CUTOFF_DOWN: {
        te_channel->filter_cutoff -= (opcode & 0xff);

        if(te_channel->filter_cutoff > 0x7ff) // unsigned int overflow
        {
            te_channel->filter_cutoff = 0;
        }

        sound_engine_filter_set_coeff(
            &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);

        break;
    }

    case TE_EFFECT_SET_RESONANCE: {
        if(tick == 0) {
            te_channel->filter_resonance = (opcode & 0xff);
            sound_engine_filter_set_coeff(
                &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
        }

        break;
    }

    case TE_EFFECT_RESONANCE_UP: {
        te_channel->filter_resonance += (opcode & 0xff);

        if(te_channel->filter_resonance > 0xff) {
            te_channel->filter_resonance = 0xff;
        }

        sound_engine_filter_set_coeff(
            &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
        break;
    }

    case TE_EFFECT_RESONANCE_DOWN: {
        te_channel->filter_resonance -= (opcode & 0xff);

        if(te_channel->filter_resonance > 0xff) {
            te_channel->filter_resonance = 0;
        }

        sound_engine_filter_set_coeff(
            &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
        break;
    }

    case TE_EFFECT_SET_RING_MOD_SRC: {
        if(tick == 0) {
            se_channel->ring_mod = (opcode & 0xff);
        }

        break;
    }

    case TE_EFFECT_SET_HARD_SYNC_SRC: {
        if(tick == 0) {
            se_channel->hard_sync = (opcode & 0xff);
        }

        break;
    }

    case TE_EFFECT_SET_ATTACK: {
        if(tick == 0) {
            se_channel->adsr.a = (opcode & 0xff);

            if(se_channel->adsr.envelope_state == ATTACK) {
                se_channel->adsr.envelope_speed =
                    envspd(tracker_engine->sound_engine, se_channel->adsr.a);
            }
        }

        break;
    }

    case TE_EFFECT_SET_DECAY: {
        if(tick == 0) {
            se_channel->adsr.d = (opcode & 0xff);

            if(se_channel->adsr.envelope_state == DECAY) {
                se_channel->adsr.envelope_speed =
                    envspd(tracker_engine->sound_engine, se_channel->adsr.d);
            }
        }

        break;
    }

    case TE_EFFECT_SET_SUSTAIN: {
        if(tick == 0) {
            se_channel->adsr.s = (opcode & 0xff);
        }

        break;
    }

    case TE_EFFECT_SET_RELEASE: {
        if(tick == 0) {
            se_channel->adsr.r = (opcode & 0xff);

            if(se_channel->adsr.envelope_state == RELEASE) {
                se_channel->adsr.envelope_speed =
                    envspd(tracker_engine->sound_engine, se_channel->adsr.r);
            }
        }

        break;
    }

    case TE_EFFECT_PROGRAM_RESTART: {
        if(tick == 0) {
            te_channel->program_counter = 0;
            te_channel->program_loop = 0;
            te_channel->program_period = 0;
            te_channel->program_tick = 0;
        }

        break;
    }

    case TE_EFFECT_SET_RATE: {
        if(tick == 0 && (opcode & 0xff) > 0) {
            tracker_engine_set_rate(opcode & 0xff);
        }

        break;
    }

    case TE_EFFECT_PORTA_UP_SEMITONE: {
        uint32_t prev = te_channel->note;

        te_channel->note += ((opcode & 0xff) << 8);
        if(prev > te_channel->note) te_channel->note = 0xffff;

        te_channel->target_note = te_channel->note;
        break;
    }

    case TE_EFFECT_PORTA_DOWN_SEMITONE: {
        int32_t prev = te_channel->note;

        te_channel->note -= ((opcode & 0xff) << 8);
        if(prev < te_channel->note) te_channel->note = 0;

        te_channel->target_note = te_channel->note;
        break;
    }

    case TE_EFFECT_PITCH: {
        te_channel->finetune_note = ((int16_t)(opcode & 0xff) - 0x80) * 2;
        break;
    }

    case TE_EFFECT_ARPEGGIO_ABS: {
        te_channel->arpeggio_note = 0;
        te_channel->fixed_note = ((opcode & 0xff) << 8);

        break;
    }

    case TE_EFFECT_TRIGGER_RELEASE: {
        if(tick == (opcode & 0xff))
        {
            sound_engine_enable_gate(tracker_engine->sound_engine, se_channel, 0);
        }

        break;
    }

    default:
        break;
    }
}