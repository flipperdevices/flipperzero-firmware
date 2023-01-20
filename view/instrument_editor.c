#include "instrument_editor.h"
#include "pattern_editor.h"

#include <flizzer_tracker_icons.h>

void draw_inst_flag(FlizzerTrackerApp *tracker, Canvas *canvas, uint8_t focus, uint8_t param, const char *text, uint8_t x, uint8_t y, uint16_t flags, uint16_t mask)
{
    canvas_draw_icon(canvas, x, y - 5, ((flags & mask) ? &I_checkbox_checked : &I_checkbox_empty));
    canvas_draw_str(canvas, x + 6, y, text);

    if (tracker->focus == focus && tracker->selected_param == param && tracker->editing)
    {
        if (text[strlen(text) - 1] == ':')
        {
            canvas_draw_box(canvas, x + 5, y - 6, strlen(text) * 4 - 1, 7);
        }

        else
        {
            canvas_draw_box(canvas, x + 5, y - 6, strlen(text) * 4 + 1, 7);
        }
    }
}

void draw_inst_text_one_digit(FlizzerTrackerApp *tracker, Canvas *canvas, uint8_t focus, uint8_t param, const char *text, uint8_t x, uint8_t y, uint8_t value) // text MUST end with semicolon
{
    canvas_draw_str(canvas, x, y, text);
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%01X", (value & 0xF));
    canvas_draw_str(canvas, x + strlen(text) * 4 - 2, y, buffer);

    if (tracker->focus == focus && tracker->selected_param == param && tracker->editing)
    {
        canvas_draw_box(canvas, x + strlen(text) * 4 - 3, y - 6, 5, 7);
    }
}

void draw_inst_text_two_digits(FlizzerTrackerApp *tracker, Canvas *canvas, uint8_t focus, uint8_t param, const char *text, uint8_t x, uint8_t y, uint8_t value) // text MUST end with semicolon
{
    canvas_draw_str(canvas, x, y, text);
    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%02X", value);
    canvas_draw_str(canvas, x + strlen(text) * 4 - 2, y, buffer);

    if (tracker->focus == focus && tracker->selected_param == param && tracker->editing)
    {
        canvas_draw_box(canvas, x + strlen(text) * 4 + 4 * tracker->current_digit - 3, y - 6, 5, 7);
    }
}

static const char *filter_types[] =
    {
        "NONE",
        "LOW",
        "HIGH",
        "BAND",
};

static const char *instrument_editor_params_description[] =
    {
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
        "SYNC OSCLLATORS ON KEYDOWN",
        "ENABLE VIBRATO",
        "VIBRATO SPEED",
        "VIBRATO DEPTH",
        "VIBRATO DELAY (IN TICKS)",
        "ENABLE PWM",
        "PWM SPEED",
        "PWM DEPTH",
        "PWM DELAY (IN TICKS)",
        "PROG.PERIOD (00 ACTS SAME AS 01)",
};

void draw_instrument_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    SoundEngineChannel *se_channel = &tracker->sound_engine.channel[0];
    if (!(se_channel->flags & SE_ENABLE_GATE) && tracker->tracker_engine.song == NULL)
    {
        stop();
        tracker->tracker_engine.playing = false;
        tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
    }

    char buffer[30];
    Instrument *inst = tracker->song.instrument[tracker->current_instrument];
    uint8_t shift = tracker->inst_editor_shift;

    if (shift == 0)
    {
        snprintf(buffer, sizeof(buffer), "INST:%c", to_char(tracker->current_instrument));
        draw_generic_n_digit_field(tracker, canvas, EDIT_INSTRUMENT, INST_CURRENTINSTRUMENT, buffer, 0, 5 - shift, 1);
        snprintf(buffer, sizeof(buffer), "%s", tracker->song.instrument[tracker->current_instrument]->name);
        draw_generic_n_digit_field(tracker, canvas, EDIT_INSTRUMENT, INST_INSTRUMENTNAME, buffer, 4 * 7, 5 - shift, 1);
    }

    snprintf(buffer, sizeof(buffer), "NOTE:%s", notename(inst->base_note));
    canvas_draw_str(canvas, 0, 11 - shift, buffer);

    if (tracker->editing && tracker->focus == EDIT_INSTRUMENT && tracker->selected_param == INST_CURRENT_NOTE)
    {
        if (tracker->current_digit)
        {
            canvas_draw_box(canvas, 19 + 2 * 4, 5 - shift, 5, 7);
        }

        else
        {
            canvas_draw_box(canvas, 19, 5 - shift, 5 + 4, 7);
        }
    }

    snprintf(buffer, sizeof(buffer), "FINE:%+02d", inst->finetune);
    canvas_draw_str(canvas, 37, 11 - shift, buffer);

    if (tracker->editing && tracker->focus == EDIT_INSTRUMENT && tracker->selected_param == INST_FINETUNE)
    {
        if (tracker->current_digit)
        {
            canvas_draw_box(canvas, 60 + 4, 5 - shift, 5, 7);
        }

        else
        {
            canvas_draw_box(canvas, 60, 5 - shift, 5, 7);
        }
    }

    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_SLIDESPEED, "SL.SPD:", 0, 17 - shift, inst->slide_speed);

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_SETPW, "PW:", 36, 17 - shift, inst->flags, TE_SET_PW);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_PW, "", 54, 17 - shift, inst->pw);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_SETCUTOFF, "CUT", 62, 17 - shift, inst->flags, TE_SET_CUTOFF);

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_NOISE, "N", 0, 23 - shift, inst->waveform, SE_WAVEFORM_NOISE);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_PULSE, "P", 10, 23 - shift, inst->waveform, SE_WAVEFORM_PULSE);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_TRIANGLE, "T", 20, 23 - shift, inst->waveform, SE_WAVEFORM_TRIANGLE);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_SAWTOOTH, "S", 30, 23 - shift, inst->waveform, SE_WAVEFORM_SAW);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_NOISE_METAL, "M", 40, 23 - shift, inst->waveform, SE_WAVEFORM_NOISE_METAL);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_WAVE_SINE, "SINE", 50, 23 - shift, inst->waveform, SE_WAVEFORM_SINE);

    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_ATTACK, "A:", 0, 29 - shift, inst->adsr.a);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_DECAY, "D:", 16, 29 - shift, inst->adsr.d);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_SUSTAIN, "S:", 32, 29 - shift, inst->adsr.s);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_RELEASE, "R:", 48, 29 - shift, inst->adsr.r);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_VOLUME, "V:", 64, 29 - shift, inst->adsr.volume);

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLEFILTER, "FIL", 0, 35 - shift, inst->sound_engine_flags, SE_ENABLE_FILTER);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_FILTERCUTOFF, "CUT:", 20, 35 - shift, inst->filter_cutoff);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_FILTERRESONANCE, "RES:", 44, 35 - shift, inst->filter_resonance);

    snprintf(buffer, sizeof(buffer), "TYPE:%s", filter_types[inst->filter_type]);
    canvas_draw_str(canvas, 0, 41 - shift, buffer);

    if (tracker->editing && tracker->focus == EDIT_INSTRUMENT && tracker->selected_param == INST_FILTERTYPE)
    {
        canvas_draw_box(canvas, 19, 35 - shift, strlen(filter_types[inst->filter_type]) * 4 + 1, 7);
    }

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLERINGMOD, "R:", 38, 41 - shift, inst->sound_engine_flags, SE_ENABLE_RING_MOD);
    draw_inst_text_one_digit(tracker, canvas, EDIT_INSTRUMENT, INST_RINGMODSRC, "", 52, 41 - shift, inst->ring_mod);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLEHARDSYNC, "H:", 56, 41 - shift, inst->sound_engine_flags, SE_ENABLE_HARD_SYNC);
    draw_inst_text_one_digit(tracker, canvas, EDIT_INSTRUMENT, INST_HARDSYNCSRC, "", 70, 41 - shift, inst->hard_sync);

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_RETRIGGERONSLIDE, "SL.RETRIG", 0, 47 - shift, inst->flags, TE_RETRIGGER_ON_SLIDE);
    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLEKEYSYNC, "KSYNC", 44, 47 - shift, inst->sound_engine_flags, SE_ENABLE_KEYDOWN_SYNC);

    draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLEVIBRATO, "VIB", 0, 53 - shift, inst->flags, TE_ENABLE_VIBRATO);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_VIBRATOSPEED, "S:", 20, 53 - shift, inst->vibrato_speed);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_VIBRATODEPTH, "D:", 36, 53 - shift, inst->vibrato_depth);
    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_VIBRATODELAY, "DEL:", 52, 53 - shift, inst->vibrato_delay);

    if (shift >= 6)
    {
        draw_inst_flag(tracker, canvas, EDIT_INSTRUMENT, INST_ENABLEPWM, "PWM", 0, 59 - shift, inst->flags, TE_ENABLE_PWM);
        draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_PWMSPEED, "S:", 20, 59 - shift, inst->pwm_speed);
        draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_PWMDEPTH, "D:", 36, 59 - shift, inst->pwm_depth);
        draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_PWMDELAY, "DEL:", 52, 59 - shift, inst->pwm_delay);
    }

    draw_inst_text_two_digits(tracker, canvas, EDIT_INSTRUMENT, INST_PROGRAMEPERIOD, "P.PERIOD:", 84, 56, inst->program_period);

    canvas_draw_line(canvas, 0, 57, 127, 57);
    canvas_draw_line(canvas, 82, 0, 82, 56);
    canvas_draw_line(canvas, 83, 49, 127, 49);

    if (tracker->focus == EDIT_INSTRUMENT)
    {
        canvas_draw_str(canvas, 0, 64, instrument_editor_params_description[tracker->selected_param]);
    }
}

void draw_instrument_program_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    UNUSED(canvas);
    UNUSED(tracker);
}