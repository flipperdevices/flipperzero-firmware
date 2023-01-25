#include "do_effects.h"
#include <furi.h>

void do_command(uint16_t opcode, TrackerEngine *tracker_engine, uint8_t channel)
{
    TrackerEngineChannel *te_channel = &tracker_engine->channel[channel];
    SoundEngineChannel *se_channel = &tracker_engine->sound_engine->channel[channel];

    UNUSED(se_channel);

    switch (opcode & 0x7f00)
    {
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

        default:
            break;
    }
}