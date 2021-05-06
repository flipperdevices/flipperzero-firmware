#include <malloc.h>
#include <stdbool.h>
#include <furi.h>
#include "irda.h"
#include "decoder_common_i.h"


static bool check_preamble(IrdaCommonDecoder *d) {
    bool rc = false;
    bool start_level = (d->level + d->tc + 1) % 2;

    // align to start at Mark timing
    if (start_level) {
        if (d->tc > 0) {
            --d->tc;
            shift_left_array(d->t, d->tc, 1);
        }
    }

    while ((!rc) && (d->tc >= 2)) {
        if ((MATCH_PREAMBLE_TIMING(d->t[0], d->spec->timings.preamb_mark))
            && (MATCH_PREAMBLE_TIMING(d->t[1], d->spec->timings.preamb_space))) {
            rc = true;
        }

        d->tc -= 2;
        shift_left_array(d->t, d->tc, 2);
    }

    return rc;
}


DecodeStatus decode_pwm(IrdaCommonDecoder* d) {
    uint32_t* t = d->t;
    uint16_t index = 0;
    uint8_t shift = 0;
    DecodeStatus status = DecodeStatusError;

    while (1) {
        // Stop bit
        if ((d->databit_cnt == d->spec->databit_len) && (d->tc == 1)) {
            if (MATCH_BIT_TIMING(t[0], d->spec->timings.bit1_mark)) {
                d->tc = 0;
                status = DecodeStatusReady;
            } else {
                status = DecodeStatusError;
            }
            break;
        }

        if (d->tc >= 2) {
            index = d->databit_cnt / 8;
            shift = d->databit_cnt % 8;   // LSB first
            if (!shift)
                d->data[index] = 0;
            if (MATCH_BIT_TIMING(t[0], d->spec->timings.bit1_mark)
                && MATCH_BIT_TIMING(t[1], d->spec->timings.bit1_space)) {
                d->data[index] |= (0x1 << shift);           // add 1
            } else if (MATCH_BIT_TIMING(t[0], d->spec->timings.bit0_mark)
                && MATCH_BIT_TIMING(t[1], d->spec->timings.bit0_space)) {
                (void) d->data[index];                      // add 0
            } else {
                status = DecodeStatusError;
                break;
            }
            ++d->databit_cnt;
            d->tc -= 2;
            shift_left_array(d->t, d->tc, 2);
        } else {
            status = DecodeStatusOk;
            break;
        }
    }

    return status;
}

const IrdaMessage* decode_common(IrdaCommonDecoder *d, bool level, uint32_t duration) {
    const IrdaMessage* im = 0;
    DecodeStatus status = DecodeStatusError;

    if (d->level == level) {
        d->tc = 0;
        furi_assert(0);
    }
    d->level = level;   // start with high level (Space timing)

    d->t[d->tc] = duration;
    d->tc++;
    furi_assert(d->tc <= sizeof(d->t));

    while(1) {
        switch (d->state) {
        case IrdaCommonStateWaitPreamble:
            if (check_preamble(d)) {
                d->state = IrdaCommonStateDecode;
            }
            break;
        case IrdaCommonStateDecode:
            status = d->spec->decode(d);
            if (status == DecodeStatusReady) {
                if (d->spec->interpret(d)) {
                    im = &d->im;
                    d->state = IrdaCommonStateProcessRepeat;
                } else {
                    d->state = IrdaCommonStateWaitPreamble;
                }
                d->databit_cnt = 0;
            } else if (status == DecodeStatusError) {
                d->state = IrdaCommonStateWaitPreamble;
                continue;
            }
            break;
        case IrdaCommonStateProcessRepeat:
            if (!d->spec->decode_repeat) {
                d->state = IrdaCommonStateWaitPreamble;
                continue;
            }
            status = d->spec->decode_repeat(d);
            if (status == DecodeStatusError) {
                d->state = IrdaCommonStateWaitPreamble;
                continue;
            } else if (status == DecodeStatusReady) {
                d->im.repeat = true;
                im = &d->im;
            }
            break;
        }
        break;
    }

    return im;
}

void* common_decoder_init(const IrdaCommonProtocolSpec *spec) {
    uint32_t alloc_size = sizeof(IrdaCommonDecoder)
                          + spec->databit_len / 8
                          + !!(spec->databit_len % 8);
    IrdaCommonDecoder* d = malloc(alloc_size);
    memset(d, 0, alloc_size);
    d->spec = spec;
    return d;
}

void common_decoder_fini(void* d) {
    free(d);
}

