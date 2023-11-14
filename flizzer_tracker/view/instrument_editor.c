#include "instrument_editor.h"
#include "pattern_editor.h"

#include "../macros.h"
#include "opcode_description.h"

#include <flizzer_tracker_icons.h>

void draw_inst_flag(
    FlizzerTrackerApp* tracker,
    Canvas* canvas,
    uint8_t focus,
    uint8_t param,
    const char* text,
    uint8_t x,
    uint8_t y,
    uint16_t flags,
    uint16_t mask) {
    canvas_draw_icon(canvas, x, y - 5, ((flags & mask) ? &I_checkbox_checked : &I_checkbox_empty));
    canvas_draw_str(canvas, x + 6, y, text);

    if(tracker->focus == focus && tracker->selected_param == param && tracker->editing) {
        if(text[strlen(text) - 1] == ':') {
            canvas_draw_box(canvas, x + 5, y - 6, strlen(text) * 4 - 1, 7);
        }

        else {
            canvas_draw_box(canvas, x + 5, y - 6, strlen(text) * 4 + 1, 7);
        }
    }

    if(tracker->focus == focus && tracker->selected_param == param && !(tracker->editing)) {
        if(text[strlen(text) - 1] == ':') {
            canvas_draw_frame(canvas, x + 5, y - 6, strlen(text) * 4 - 1, 7);
        }

        else {
            canvas_draw_frame(canvas, x + 5, y - 6, strlen(text) * 4 + 1, 7);
        }
    }
}

void draw_inst_text_one_digit(
    FlizzerTrackerApp* tracker,
    Canvas* canvas,
    uint8_t focus,
    uint8_t param,
    const char* text,
    uint8_t x,
    uint8_t y,
    uint8_t value) // text MUST end with semicolon
{
    canvas_draw_str(canvas, x, y, text);
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%01X", (value & 0xF));
    canvas_draw_str(canvas, x + strlen(text) * 4 - 2, y, buffer);

    if(tracker->focus == focus && tracker->selected_param == param && tracker->editing) {
        canvas_draw_box(canvas, x + strlen(text) * 4 - 3, y - 6, 5, 7);
    }

    if(tracker->focus == focus && tracker->selected_param == param && !(tracker->editing)) {
        canvas_draw_frame(canvas, x + strlen(text) * 4 - 3, y - 6, 5, 7);
    }
}

void draw_inst_text_two_digits(
    FlizzerTrackerApp* tracker,
    Canvas* canvas,
    uint8_t focus,
    uint8_t param,
    const char* text,
    uint8_t x,
    uint8_t y,
    uint8_t value) // text MUST end with semicolon
{
    canvas_draw_str(canvas, x, y, text);
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%02X", value);
    canvas_draw_str(canvas, x + strlen(text) * 4 - 2, y, buffer);

    if(tracker->focus == focus && tracker->selected_param == param && tracker->editing) {
        canvas_draw_box(
            canvas, x + strlen(text) * 4 + 4 * tracker->current_digit - 3, y - 6, 5, 7);
    }

    if(tracker->focus == focus && tracker->selected_param == param && !(tracker->editing)) {
        canvas_draw_frame(
            canvas, x + strlen(text) * 4 + 4 * tracker->current_digit - 3, y - 6, 5, 7);
    }
}

static const char* filter_types[] = {
    "NONE",
    "LOW",
    "HIGH",
    "BAND",
    "LOHI",
    "HIBD",
    "LOBD",
    "ALL",
};

static const char* instrument_editor_params_description[] = {
    "CURRENT INSTRUMENT",
    "CURRENT INSTRUMENT NAME",
    "INSTRUMENT BASE NOTE",
    "INSTRUMENT FINETUNE",
    "SLIDE SPEED",
    "SET PULSE WIDTH ON KEYDOWN",
    "PULSE WIDTH",
    "SET FILTER PARAMETERS ON KEYDOWN",
    "NOISE WAVEFORM",
    "PULSE WAVEFORM",
    "TRIANGLE WAVEFORM",
    "SAWTOOTH WAVEFORM",
    "METALLIC NOISE WAVEFORM",
    "SINE WAVEFORM",
    "ENVELOPE ATTACK",
    "ENVELOPE DECAY",
    "ENVELOPE SUSTAIN",
    "ENVELOPE RELEASE",
    "ENVELOPE VOLUME",
    "ENABLE FILTER",
    "FILTER CUTOFF FREQUENCY",
    "FILTER RESONANCE",
    "FILTER TYPE (NONE=OFF)",
    "ENABLE RING MODULATION",
    "RINGMOD SOURCE CHANNEL (F=SELF)",
    "ENABLE HARD SYNC",
    "HARDSYNC SOURCE CHANNEL (F=SELF)",
    "RETRIGGER INSTRUMENT ON SLIDE",
    "SYNC OSCILLATORS ON KEYDOWN",
    "ENABLE VIBRATO",
    "VIBRATO SPEED",
    "VIBRATO DEPTH",
    "VIBRATO DELAY (IN TICKS)",
    "ENABLE PWM",
    "PWM SPEED",
    "PWM DEPTH",
    "PWM DELAY (IN TICKS)",
    "DON'T RESTART PROGRAM ON KEYDOWN",
    "PROG.PERIOD (00 = PROGRAM OFF)",
};

void draw_instrument_view(Canvas* canvas, FlizzerTrackerApp* tracker) {
    SoundEngineChannel* se_channel = &tracker->sound_engine.channel[0];
    if(!(se_channel->flags & SE_ENABLE_GATE) && tracker->tracker_engine.song == NULL) {
        stop();
        tracker->tracker_engine.playing = false;
        tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
    }

    char buffer[30];
    Instrument* inst = tracker->song.instrument[tracker->current_instrument];
    uint8_t shift = tracker->inst_editor_shift;

    if(shift < 6) {
        snprintf(buffer, sizeof(buffer), "INST:%c", to_char(tracker->current_instrument));
        draw_generic_n_digit_field(
            tracker, canvas, EDIT_INSTRUMENT, INST_CURRENTINSTRUMENT, buffer, 0, 5 - shift, 1);
        snprintf(
            buffer,
            sizeof(buffer),
            "%s",
            tracker->song.instrument[tracker->current_instrument]->name);
        draw_generic_n_digit_field(
            tracker, canvas, EDIT_INSTRUMENT, INST_INSTRUMENTNAME, buffer, 4 * 7 - 1, 5 - shift, 1);
    }

    if(shift < 12) {
        snprintf(buffer, sizeof(buffer), "NOTE:%s", notename(inst->base_note));
        canvas_draw_str(canvas, 0, 11 - shift, buffer);

        if(tracker->editing && tracker->focus == EDIT_INSTRUMENT &&
           tracker->selected_param == INST_CURRENT_NOTE) {
            if(tracker->current_digit) {
                canvas_draw_box(canvas, 19 + 2 * 4, 5 - shift, 5, 7);
            }

            else {
                canvas_draw_box(canvas, 19, 5 - shift, 5 + 4, 7);
            }
        }

        if(!(tracker->editing) && tracker->focus == EDIT_INSTRUMENT &&
           tracker->selected_param == INST_CURRENT_NOTE) {
            if(tracker->current_digit) {
                canvas_draw_frame(canvas, 19 + 2 * 4, 5 - shift, 5, 7);
            }

            else {
                canvas_draw_frame(canvas, 19, 5 - shift, 5 + 4, 7);
            }
        }

        snprintf(buffer, sizeof(buffer), "FINE:%+02d", inst->finetune);
        canvas_draw_str(canvas, 37, 11 - shift, buffer);

        if(tracker->editing && tracker->focus == EDIT_INSTRUMENT &&
           tracker->selected_param == INST_FINETUNE) {
            if(tracker->current_digit) {
                canvas_draw_box(canvas, 60 + 4, 5 - shift, 5, 7);
            }

            else {
                canvas_draw_box(canvas, 60, 5 - shift, 5, 7);
            }
        }

        if(!(tracker->editing) && tracker->focus == EDIT_INSTRUMENT &&
           tracker->selected_param == INST_FINETUNE) {
            if(tracker->current_digit) {
                canvas_draw_frame(canvas, 60 + 4, 5 - shift, 5, 7);
            }

            else {
                canvas_draw_frame(canvas, 60, 5 - shift, 5, 7);
            }
        }
    }

    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_SLIDESPEED,
        "SL.SPD:",
        0,
        17 - shift,
        inst->slide_speed);

    draw_inst_flag(
        tracker, canvas, EDIT_INSTRUMENT, INST_SETPW, "PW:", 36, 17 - shift, inst->flags, TE_SET_PW);
    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_PW, "", 54, 17 - shift, inst->pw);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_SETCUTOFF,
        "CUT",
        61,
        17 - shift,
        inst->flags,
        TE_SET_CUTOFF);

    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_NOISE,
        "N",
        0,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_NOISE);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_PULSE,
        "P",
        10,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_PULSE);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_TRIANGLE,
        "T",
        20,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_TRIANGLE);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_SAWTOOTH,
        "S",
        30,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_SAW);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_NOISE_METAL,
        "M",
        40,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_NOISE_METAL);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_WAVE_SINE,
        "SINE",
        50,
        23 - shift,
        inst->waveform,
        SE_WAVEFORM_SINE);

    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_ATTACK, "A:", 0, 29 - shift, inst->adsr.a);
    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_DECAY, "D:", 16, 29 - shift, inst->adsr.d);
    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_SUSTAIN, "S:", 32, 29 - shift, inst->adsr.s);
    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_RELEASE, "R:", 48, 29 - shift, inst->adsr.r);
    draw_inst_text_two_digits(
        tracker, canvas, EDIT_INSTRUMENT, INST_VOLUME, "V:", 64, 29 - shift, inst->adsr.volume);

    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_ENABLEFILTER,
        "FIL",
        0,
        35 - shift,
        inst->sound_engine_flags,
        SE_ENABLE_FILTER);
    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_FILTERCUTOFF,
        "CUT:",
        20,
        35 - shift,
        inst->filter_cutoff);
    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_FILTERRESONANCE,
        "RES:",
        44,
        35 - shift,
        inst->filter_resonance);

    snprintf(buffer, sizeof(buffer), "TYPE:%s", filter_types[inst->filter_type]);
    canvas_draw_str(canvas, 0, 41 - shift, buffer);

    if(tracker->editing && tracker->focus == EDIT_INSTRUMENT &&
       tracker->selected_param == INST_FILTERTYPE) {
        canvas_draw_box(
            canvas, 19, 35 - shift, strlen(filter_types[inst->filter_type]) * 4 + 1, 7);
    }

    if(!(tracker->editing) && tracker->focus == EDIT_INSTRUMENT &&
       tracker->selected_param == INST_FILTERTYPE) {
        canvas_draw_frame(
            canvas, 19, 35 - shift, strlen(filter_types[inst->filter_type]) * 4 + 1, 7);
    }

    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_ENABLERINGMOD,
        "R:",
        38,
        41 - shift,
        inst->sound_engine_flags,
        SE_ENABLE_RING_MOD);
    draw_inst_text_one_digit(
        tracker, canvas, EDIT_INSTRUMENT, INST_RINGMODSRC, "", 52, 41 - shift, inst->ring_mod);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_ENABLEHARDSYNC,
        "H:",
        56,
        41 - shift,
        inst->sound_engine_flags,
        SE_ENABLE_HARD_SYNC);
    draw_inst_text_one_digit(
        tracker, canvas, EDIT_INSTRUMENT, INST_HARDSYNCSRC, "", 70, 41 - shift, inst->hard_sync);

    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_RETRIGGERONSLIDE,
        "SL.RETRIG",
        0,
        47 - shift,
        inst->flags,
        TE_RETRIGGER_ON_SLIDE);
    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_ENABLEKEYSYNC,
        "KSYNC",
        44,
        47 - shift,
        inst->sound_engine_flags,
        SE_ENABLE_KEYDOWN_SYNC);

    draw_inst_flag(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_ENABLEVIBRATO,
        "VIB",
        0,
        53 - shift,
        inst->flags,
        TE_ENABLE_VIBRATO);
    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_VIBRATOSPEED,
        "S:",
        20,
        53 - shift,
        inst->vibrato_speed);
    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_VIBRATODEPTH,
        "D:",
        36,
        53 - shift,
        inst->vibrato_depth);
    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_VIBRATODELAY,
        "DEL:",
        52,
        53 - shift,
        inst->vibrato_delay);

    if(shift >= 6) {
        draw_inst_flag(
            tracker,
            canvas,
            EDIT_INSTRUMENT,
            INST_ENABLEPWM,
            "PWM",
            0,
            59 - shift,
            inst->flags,
            TE_ENABLE_PWM);
        draw_inst_text_two_digits(
            tracker, canvas, EDIT_INSTRUMENT, INST_PWMSPEED, "S:", 20, 59 - shift, inst->pwm_speed);
        draw_inst_text_two_digits(
            tracker, canvas, EDIT_INSTRUMENT, INST_PWMDEPTH, "D:", 36, 59 - shift, inst->pwm_depth);
        draw_inst_text_two_digits(
            tracker,
            canvas,
            EDIT_INSTRUMENT,
            INST_PWMDELAY,
            "DEL:",
            52,
            59 - shift,
            inst->pwm_delay);
    }

    if(shift >= 12) {
        draw_inst_flag(
            tracker,
            canvas,
            EDIT_INSTRUMENT,
            INST_PROGRESTART,
            "NO PROG.RESTART",
            0,
            65 - shift,
            inst->flags,
            TE_PROG_NO_RESTART);
    }

    draw_inst_text_two_digits(
        tracker,
        canvas,
        EDIT_INSTRUMENT,
        INST_PROGRAMEPERIOD,
        "P.PERIOD:",
        81,
        56,
        inst->program_period);

    canvas_draw_line(canvas, 0, 57, 127, 57);
    canvas_draw_line(canvas, 79, 0, 79, 56);
    canvas_draw_line(canvas, 80, 49, 127, 49);

    if(tracker->focus == EDIT_INSTRUMENT) {
        canvas_draw_str(
            canvas, 0, 64, instrument_editor_params_description[tracker->selected_param]);
    }
}

char command_get_char(uint16_t command) {
    if((command >> 8) < 36) {
        return to_char_array[(command >> 8)];
    }

    if(command == TE_PROGRAM_END) {
        return ':';
    }

    if((command & 0xff00) == TE_PROGRAM_JUMP) {
        return '^';
    }

    if((command & 0xff00) == TE_PROGRAM_LOOP_END) {
        return '>';
    }

    if((command & 0xff00) == TE_PROGRAM_LOOP_BEGIN) {
        return '<';
    }

    return '?';
}

void draw_program_step(Canvas* canvas, uint8_t y, FlizzerTrackerApp* tracker, uint8_t index) {
    char buffer[15];

    Instrument* inst = tracker->song.instrument[tracker->current_instrument];
    uint16_t opcode = inst->program[index];

    if(opcode != TE_PROGRAM_NOP) {
        if((opcode & 0x7f00) == TE_EFFECT_ARPEGGIO) {
            if((opcode & 0xff) != 0xf0 && (opcode & 0xff) != 0xf1) {
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "%01X %c%02X %s",
                    index,
                    command_get_char(opcode & 0x7fff),
                    (opcode & 0xff),
                    notename(my_min(
                        12 * 7 + 11,
                        (opcode & 0xff) +
                            tracker->song.instrument[tracker->current_instrument]->base_note)));
            }

            else {
                snprintf(
                    buffer,
                    sizeof(buffer),
                    "%01X %c%02X %s",
                    index,
                    command_get_char(opcode & 0x7fff),
                    (opcode & 0xff),
                    notename((opcode & 0xff)));
            }
        }

        else if((opcode & 0x7f00) == TE_EFFECT_ARPEGGIO_ABS) {
            snprintf(
                buffer,
                sizeof(buffer),
                "%01X %c%02X F.%s",
                index,
                command_get_char(opcode & 0x7fff),
                (opcode & 0xff),
                notename(opcode & 0xff));
        }

        else {
            snprintf(
                buffer,
                sizeof(buffer),
                "%01X %c%02X %s",
                index,
                command_get_char(opcode & 0x7fff),
                (opcode & 0xff),
                get_opcode_description(opcode, true) ? get_opcode_description(opcode, true) : "");
        }

        if(opcode & 0x8000) {
            if(index == 0) {
                canvas_draw_line(canvas, 84 + 4 * 4 + 2, y, 84 + 4 * 4 + 2, y - 3);
                canvas_draw_dot(canvas, 84 + 4 * 4 + 1, y - 4);
            }

            if(index > 0 && !(inst->program[index - 1] & 0x8000)) {
                canvas_draw_line(canvas, 84 + 4 * 4 + 2, y, 84 + 4 * 4 + 2, y - 3);
                canvas_draw_dot(canvas, 84 + 4 * 4 + 1, y - 4);
            }

            if(index > 0 && (inst->program[index - 1] & 0x8000)) {
                canvas_draw_line(canvas, 84 + 4 * 4 + 2, y, 84 + 4 * 4 + 2, y - 5);
            }
        }

        else {
            if(index > 0 && (inst->program[index - 1] & 0x8000)) {
                canvas_draw_line(canvas, 84 + 4 * 4 + 2, y - 3, 84 + 4 * 4 + 2, y - 5);
                canvas_draw_dot(canvas, 84 + 4 * 4 + 1, y - 2);
            }
        }
    }

    else {
        snprintf(buffer, sizeof(buffer), "%01X ---", index);
    }

    canvas_draw_str(canvas, 81, y, buffer);
}

void draw_instrument_program_view(Canvas* canvas, FlizzerTrackerApp* tracker) {
    Instrument* inst = tracker->song.instrument[tracker->current_instrument];

    for(uint8_t i = tracker->program_position;
        i < my_min(INST_PROG_LEN, tracker->program_position + 8);
        i++) {
        draw_program_step(canvas, 6 + 6 * i - tracker->program_position * 6, tracker, i);

        if(i == tracker->current_program_step && tracker->focus == EDIT_PROGRAM) {
            if(tracker->editing) {
                canvas_draw_box(
                    canvas,
                    80 + 8 + tracker->current_digit * 4,
                    6 * i - tracker->program_position * 6,
                    5,
                    7);
            }

            else {
                canvas_draw_box(canvas, 80, 6 * i - tracker->program_position * 6, 5, 7);
            }
        }
    }

    // draw arrow pointing at current program step

    for(uint8_t i = 0; i < SONG_MAX_CHANNELS; i++) {
        if(tracker->tracker_engine.channel[i].instrument == inst &&
           (tracker->tracker_engine.channel[i].channel_flags & TEC_PROGRAM_RUNNING) &&
           (tracker->tracker_engine.sound_engine->channel[i].flags & SE_ENABLE_GATE)) {
            if(tracker->tracker_engine.channel[i].program_tick >= tracker->program_position &&
               tracker->tracker_engine.channel[i].program_tick < tracker->program_position + 8) {
                canvas_draw_str(
                    canvas,
                    85,
                    6 * tracker->tracker_engine.channel[i].program_tick -
                        tracker->program_position * 6 + 6,
                    ">");
                break;
            }
        }
    }

    if(tracker->focus == EDIT_PROGRAM) {
        uint16_t opcode = (inst->program[tracker->current_program_step] & 0x7fff);
        canvas_draw_str(
            canvas,
            0,
            64,
            get_opcode_description(opcode, false) ? get_opcode_description(opcode, false) : "");
    }
}