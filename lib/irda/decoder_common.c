#include <stdbool.h>
#include <furi.h>
#include "irda_i.h"
#include "decoder_common_i.h"


static bool check_preamble(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    bool result = false;
    bool start_level = (decoder->level + decoder->timings_cnt + 1) % 2;

    // align to start at Mark timing
    if (start_level) {
        if (decoder->timings_cnt > 0) {
            --decoder->timings_cnt;
            shift_left_array(decoder->timings, decoder->timings_cnt, 1);
        }
    }

    while ((!result) && (decoder->timings_cnt >= 2)) {
        if ((MATCH_PREAMBLE_TIMING(decoder->timings[0], decoder->protocol->timings.preamb_mark))
            && (MATCH_PREAMBLE_TIMING(decoder->timings[1], decoder->protocol->timings.preamb_space))) {
            result = true;
        }

        decoder->timings_cnt -= 2;
        shift_left_array(decoder->timings, decoder->timings_cnt, 2);
    }

    return result;
}


DecodeStatus decode_pwm(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    uint32_t* timings = decoder->timings;
    uint16_t index = 0;
    uint8_t shift = 0;
    DecodeStatus status = DecodeStatusError;

    while (1) {
        // Stop bit
        if ((decoder->databit_cnt == decoder->protocol->databit_len) && (decoder->timings_cnt == 1)) {
            if (MATCH_BIT_TIMING(timings[0], decoder->protocol->timings.bit1_mark)) {
                decoder->timings_cnt = 0;
                status = DecodeStatusReady;
            } else {
                status = DecodeStatusError;
            }
            break;
        }

        if (decoder->timings_cnt >= 2) {
            index = decoder->databit_cnt / 8;
            shift = decoder->databit_cnt % 8;   // LSB first
            if (!shift)
                decoder->data[index] = 0;
            if (MATCH_BIT_TIMING(timings[0], decoder->protocol->timings.bit1_mark)
                && MATCH_BIT_TIMING(timings[1], decoder->protocol->timings.bit1_space)) {
                decoder->data[index] |= (0x1 << shift);           // add 1
            } else if (MATCH_BIT_TIMING(timings[0], decoder->protocol->timings.bit0_mark)
                && MATCH_BIT_TIMING(timings[1], decoder->protocol->timings.bit0_space)) {
                (void) decoder->data[index];                      // add 0
            } else {
                status = DecodeStatusError;
                break;
            }
            ++decoder->databit_cnt;
            decoder->timings_cnt -= 2;
            shift_left_array(decoder->timings, decoder->timings_cnt, 2);
        } else {
            status = DecodeStatusOk;
            break;
        }
    }

    return status;
}

IrdaMessage* decode_common(IrdaCommonDecoder* decoder, bool level, uint32_t duration) {
    furi_assert(decoder);

    IrdaMessage* message = 0;
    DecodeStatus status = DecodeStatusError;

    if (decoder->level == level) {
        furi_assert(0);
        decoder->timings_cnt = 0;
    }
    decoder->level = level;   // start with high level (Space timing)

    decoder->timings[decoder->timings_cnt] = duration;
    decoder->timings_cnt++;
    furi_check(decoder->timings_cnt <= sizeof(decoder->timings));

    while(1) {
        switch (decoder->state) {
        case IrdaCommonStateWaitPreamble:
            if (check_preamble(decoder)) {
                decoder->state = IrdaCommonStateDecode;
                decoder->databit_cnt = 0;
            }
            break;
        case IrdaCommonStateDecode:
            status = decoder->protocol->decode(decoder);
            if (status == DecodeStatusReady) {
                if (decoder->protocol->interpret(decoder)) {
                    message = &decoder->message;
                    decoder->state = IrdaCommonStateProcessRepeat;
                } else {
                    decoder->state = IrdaCommonStateWaitPreamble;
                }
            } else if (status == DecodeStatusError) {
                decoder->state = IrdaCommonStateWaitPreamble;
                continue;
            }
            break;
        case IrdaCommonStateProcessRepeat:
            if (!decoder->protocol->decode_repeat) {
                decoder->state = IrdaCommonStateWaitPreamble;
                continue;
            }
            status = decoder->protocol->decode_repeat(decoder);
            if (status == DecodeStatusError) {
                decoder->state = IrdaCommonStateWaitPreamble;
                continue;
            } else if (status == DecodeStatusReady) {
                decoder->message.repeat = true;
                message = &decoder->message;
            }
            break;
        }
        break;
    }

    return message;
}

void* common_decoder_init(const IrdaCommonProtocolSpec* protocol) {
    furi_assert(protocol);

    uint32_t alloc_size = sizeof(IrdaCommonDecoder)
                          + protocol->databit_len / 8
                          + !!(protocol->databit_len % 8);
    IrdaCommonDecoder* decoder = furi_alloc(alloc_size);
    memset(decoder, 0, alloc_size);
    decoder->protocol = protocol;
    return decoder;
}

void common_decoder_fini(void* decoder) {
    furi_assert(decoder);

    free(decoder);
}

