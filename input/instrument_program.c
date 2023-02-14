#include "instrument_program.h"
#include "../macros.h"

void instrument_program_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
    if(event->input.key == InputKeyOk && event->input.type == InputTypeShort) {
        tracker->editing = !(tracker->editing);
        return;
    }

    if(event->input.key == InputKeyRight && event->input.type == InputTypeShort &&
       tracker->editing) {
        tracker->current_digit = my_min(2, tracker->current_digit + 1);
        return;
    }

    if(event->input.key == InputKeyOk && event->input.type == InputTypeLong && tracker->editing) {
        Instrument* inst = tracker->song.instrument[tracker->current_instrument];

        if(tracker->current_program_step < INST_PROG_LEN - 1) {
            if((inst->program[tracker->current_program_step] & 0x7fff) < TE_PROGRAM_LOOP_BEGIN &&
               ((inst->program[tracker->current_program_step + 1] & 0x7fff) <
                    TE_PROGRAM_LOOP_BEGIN ||
                (inst->program[tracker->current_program_step + 1] & 0x7f00) ==
                    TE_PROGRAM_LOOP_END)) // so we can unite with loop end as in klystrack
            {
                inst->program[tracker->current_program_step] ^= 0x8000; // flipping unite bit
            }
        }

        return;
    }

    if(event->input.key == InputKeyLeft && event->input.type == InputTypeShort &&
       tracker->editing) {
        tracker->current_digit = fmax(0, (int16_t)tracker->current_digit - 1);
        return;
    }

    if(event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
       tracker->editing) {
        Instrument* inst = tracker->song.instrument[tracker->current_instrument];
        inst->program[tracker->current_program_step] = TE_PROGRAM_NOP;
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            if((int16_t)tracker->current_program_step - 1 >= 0) {
                tracker->current_program_step--;

                if(tracker->program_position > tracker->current_program_step) {
                    tracker->program_position = tracker->current_program_step;
                }
            }

            else {
                tracker->current_program_step = INST_PROG_LEN - 1;

                tracker->program_position = INST_PROG_LEN - 1 - 7;
            }
        }

        if(tracker->editing) {
            Instrument* inst = tracker->song.instrument[tracker->current_instrument];
            uint16_t opcode = inst->program[tracker->current_program_step];

            switch(tracker->current_digit) {
            case 0: // MSB
            {
                uint8_t param = ((opcode & 0x7f00) >> 8);

                if(param < 0xff) {
                    param++;
                }

                if((inst->program[tracker->current_program_step] & 0x7fff) == TE_PROGRAM_NOP) {
                    param = 0;
                    inst->program[tracker->current_program_step] = 0;
                }

                param &= 0x7f;

                inst->program[tracker->current_program_step] &= 0x80ff;
                inst->program[tracker->current_program_step] |= ((uint16_t)param << 8);

                break;
            }

            case 1: // upper digit of param, e.g. eXx
            {
                int8_t nibble = ((opcode & 0x00f0) >> 4);

                if(nibble + 1 <= 0xf) {
                    nibble++;
                }

                else {
                    nibble = 0;
                }

                inst->program[tracker->current_program_step] &= 0xff0f;
                inst->program[tracker->current_program_step] |= (nibble << 4);

                break;
            }

            case 2: // lower digit of param, e.g. exX
            {
                int8_t nibble = (opcode & 0x000f);

                if(nibble + 1 <= 0xf) {
                    nibble++;
                }

                else {
                    nibble = 0;
                }

                inst->program[tracker->current_program_step] &= 0xfff0;
                inst->program[tracker->current_program_step] |= nibble;

                break;
            }

            default:
                break;
            }
        }

        return;
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            if(tracker->current_program_step + 1 < INST_PROG_LEN) {
                tracker->current_program_step++;

                if(tracker->program_position < tracker->current_program_step - 7) {
                    tracker->program_position = tracker->current_program_step - 7;
                }
            }

            else {
                tracker->current_program_step = 0;

                tracker->program_position = 0;
            }
        }

        if(tracker->editing) {
            Instrument* inst = tracker->song.instrument[tracker->current_instrument];
            uint16_t opcode = inst->program[tracker->current_program_step];

            switch(tracker->current_digit) {
            case 0: // MSB
            {
                uint8_t param = ((opcode & 0x7f00) >> 8);

                if(param < (TE_PROGRAM_JUMP >> 8) && param > 0) {
                    param--;

                    inst->program[tracker->current_program_step] &= 0x80ff;
                    inst->program[tracker->current_program_step] |= ((uint16_t)param << 8);
                }

                if((inst->program[tracker->current_program_step] & 0x7f00) == TE_PROGRAM_JUMP &&
                   (inst->program[tracker->current_program_step] & 0x7fff) != TE_PROGRAM_END &&
                   (inst->program[tracker->current_program_step] & 0x7fff) != TE_PROGRAM_NOP) {
                    inst->program[tracker->current_program_step] =
                        TE_PROGRAM_LOOP_END |
                        (inst->program[tracker->current_program_step] & 0x8000);
                }

                if((inst->program[tracker->current_program_step] & 0x7fff) == TE_PROGRAM_END) {
                    // param = (TE_PROGRAM_JUMP >> 8);
                    inst->program[tracker->current_program_step] =
                        TE_PROGRAM_JUMP | (inst->program[tracker->current_program_step] & 0x8000);
                }

                if((inst->program[tracker->current_program_step] & 0x7fff) == TE_PROGRAM_NOP) {
                    // param = (TE_PROGRAM_END >> 8);
                    inst->program[tracker->current_program_step] =
                        TE_PROGRAM_END | (inst->program[tracker->current_program_step] & 0x8000);
                }

                if((inst->program[tracker->current_program_step] & 0x7f00) ==
                   (TE_PROGRAM_LOOP_BEGIN - 0x100)) {
                    // param = (TE_PROGRAM_END >> 8);
                    inst->program[tracker->current_program_step] =
                        TE_EFFECT_TRIGGER_RELEASE |
                        (inst->program[tracker->current_program_step] & 0x8000);
                }

                break;
            }

            case 1: // upper digit of param, e.g. eXx
            {
                int8_t nibble = ((opcode & 0x00f0) >> 4);

                if(nibble - 1 >= 0) {
                    nibble--;
                }

                else {
                    nibble = 0xf;
                }

                inst->program[tracker->current_program_step] &= 0xff0f;
                inst->program[tracker->current_program_step] |= (nibble << 4);

                break;
            }

            case 2: // lower digit of param, e.g. exX
            {
                int8_t nibble = (opcode & 0x000f);

                if(nibble - 1 >= 0) {
                    nibble--;
                }

                else {
                    nibble = 0xf;
                }

                inst->program[tracker->current_program_step] &= 0xfff0;
                inst->program[tracker->current_program_step] |= nibble;

                break;
            }

            default:
                break;
            }
        }

        return;
    }
}