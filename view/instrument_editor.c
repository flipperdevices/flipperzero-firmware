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
    snprintf(buffer, sizeof(buffer), "%01X", value);
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

void draw_instrument_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    SoundEngineChannel *se_channel = &tracker->sound_engine.channel[0];
    if (!(se_channel->flags & SE_ENABLE_GATE))
    {
        stop();
        tracker->tracker_engine.playing = false;
        tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
    }

    char buffer[20];
    Instrument *inst = tracker->song.instrument[tracker->current_instrument];
    uint8_t shift = tracker->inst_editor_shift;

    if (shift < 6)
    {
        snprintf(buffer, sizeof(buffer), "INST:%c", to_char(tracker->current_instrument));
        draw_generic_n_digit_field(tracker, canvas, EDIT_INSTRUMENT, INST_CURRENTINSTRUMENT, buffer, 0, 5 - shift, 1);
        snprintf(buffer, sizeof(buffer), "%s", tracker->song.instrument[tracker->current_instrument]->name);
        draw_generic_n_digit_field(tracker, canvas, EDIT_INSTRUMENT, INST_INSTRUMENTNAME, buffer, 4 * 7, 5 - shift, 1);
    }

    snprintf(buffer, sizeof(buffer), "NOTE:%s", notename(inst->base_note));
    canvas_draw_str(canvas, 0, 11, buffer);

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
    canvas_draw_str(canvas, 37, 11, buffer);

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
}

void draw_instrument_program_view(Canvas *canvas, FlizzerTrackerApp *tracker)
{
    UNUSED(canvas);
    UNUSED(tracker);
}