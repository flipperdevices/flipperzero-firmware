#include "opcode_description.h"
#include <stdint.h>

static const OpcodeDescription opcode_desc[] =
    {
        {TE_EFFECT_ARPEGGIO, 0x7f00, "RELATIVE ARPEGGIO NOTE", ""},
        {TE_EFFECT_PORTAMENTO_UP, 0x7f00, "PORTAMENTO UP", "PORTUP"},
        {TE_EFFECT_PORTAMENTO_DOWN, 0x7f00, "PORTAMENTO DOWN", "PORTDN"},
        {TE_EFFECT_SLIDE, 0x7f00, "SLIDE", "SLIDE"},
        {TE_EFFECT_VIBRATO, 0x7f00, "VIBRATO", "VIB"},
        {TE_EFFECT_PWM, 0x7f00, "PULSE WIDTH MODIFICATION", "PWM"},

        {TE_EFFECT_SET_PW, 0x7f00, "SET PULSE WIDTH", "SET PW"},
        {TE_EFFECT_PW_DOWN, 0x7f00, "PULSE WIDTH DOWN", "PWDOWN"},
        {TE_EFFECT_PW_UP, 0x7f00, "PULSE WIDTH UP", "PW UP"},
        {TE_EFFECT_SET_CUTOFF, 0x7f00, "SET FILTER CUTOFF", "F.CUT"},

        {TE_EFFECT_VOLUME_FADE, 0x7f00, "VOLUME FADE", "V.FADE"},
        {TE_EFFECT_SET_WAVEFORM, 0x7f00, "SET WAVEFORM", "S.WAVE"},
        {TE_EFFECT_SET_VOLUME, 0x7f00, "SET VOLUME", "VOLUME"},
        {TE_EFFECT_SKIP_PATTERN, 0x7f00, "SKIP PATTERN", "P.SKIP"},
        {TE_EFFECT_EXT_NOTE_DELAY, 0x7ff0, "NOTE DELAY", "N.DEL."},
        {TE_EFFECT_SET_SPEED_PROG_PERIOD, 0x7f00, "SET SPEED (PROG.PER.IN PROGRAM)", "P.PER."},
        {TE_PROGRAM_LOOP_BEGIN, 0x7f00, "PROGRAM LOOP BEGIN", "L.BEG."},
        {TE_PROGRAM_LOOP_END, 0x7f00, "PROGRAM LOOP END", "L.END"},

        {TE_PROGRAM_NOP, 0x7fff, "NO OPERATION", ""},
        {TE_PROGRAM_END, 0x7fff, "PROGRAM END", "PR.END"},
        {TE_PROGRAM_JUMP, 0x7f00, "JUMP TO POSITION", "GOTO"},
        {0, 0, NULL, NULL},
};

char *get_opcode_description(uint16_t opcode, bool short_description)
{
    for (int i = 0; opcode_desc[i].name != NULL; i++)
    {
        if (opcode_desc[i].opcode == (opcode & opcode_desc[i].mask))
        {
            return short_description ? opcode_desc[i].shortname : opcode_desc[i].name;
        }
    }

    return NULL;
}