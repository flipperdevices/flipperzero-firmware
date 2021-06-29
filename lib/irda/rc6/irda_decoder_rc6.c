#include "irda.h"
#include "irda_common_decoder_i.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <furi.h>
#include "../irda_i.h"

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))


typedef struct {
    IrdaCommonDecoder* common_decoder;
    bool toggle;
} IrdaRc6Decoder;

static bool interpret_rc6(IrdaCommonDecoder* decoder);
static DecodeStatus irda_common_decode_manchester(IrdaCommonDecoder* decoder);
static DecodeStatus irda_common_decode_rc6(IrdaCommonDecoder* decoder);

static const IrdaCommonProtocolSpec protocol_rc6 = {
    .timings = {
        .preamble_mark = 2666,
        .preamble_space = 888,
        .bit1_mark = 444,
        .bit1_space = 0,
        .bit0_mark = 0,
        .bit0_space = 0,
        .preamble_tolerance = IRDA_NEC_PREAMBLE_TOLERANCE,
        .bit_tolerance = IRDA_NEC_BIT_TOLERANCE,
    },
    .databit_len = 1 + 3 + 1 + 8 + 8,   // start_bit + 3 mode bits, + 1 toggle bit (x2 timing) + 8 address + 8 command
    .manchester_inverse_level = false,
//    .manchester_starts_from_middle = false,
    .decode = irda_common_decode_rc6,
    .interpret = interpret_rc6,
    .decode_repeat = NULL,
};

static uint8_t reverse(uint8_t value);
static uint8_t reverse(uint8_t value) {
    uint8_t reverse_value = 0;
    for (int i = 0; i < sizeof(value) * 8; ++i) {
        reverse_value |= (value & (0x01 << i)) ? 1 << (7 - i) : 0;
    }

    return reverse_value;
}

#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

static bool interpret_rc6(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);

    bool result = false;
    uint32_t* data = (void*) &decoder->data[0];
    // MSB first
    uint8_t address = reverse((uint8_t) (*data >> 5));
    uint8_t command = reverse((uint8_t) (*data >> 13));
    bool start_bit = *data & 0x01;
    bool toggle = !!(*data & 0x10);
//    uint8_t mode = (*data >> 1) & 0x7;

    if (start_bit == 1) {
//    if ((start_bit == 1) && (mode == 0)) {    // tmp
        IrdaMessage* message = &decoder->message;
        IrdaRc6Decoder *rc6_decoder = decoder->context;
        bool *prev_toggle = &rc6_decoder->toggle;
        furi_assert(prev_toggle == &rc6_decoder->toggle);
        furi_assert(*prev_toggle == rc6_decoder->toggle);
        if ((message->address == address)
            && (message->command == command)
            && (message->protocol == IrdaProtocolRC6)) {
            message->repeat = (toggle == *prev_toggle);
        } else 
            message->repeat = false;
        *prev_toggle = toggle;
        message->command = command;
        message->address = address;
        printf("result: adr: 0x%lX, cmd: 0x%lX, repeat %d\r\n", message->address, message->command, message->repeat);
        result = true;
    }

    return result;
}

void* irda_decoder_rc6_alloc(void) {
    IrdaRc6Decoder* decoder = furi_alloc(sizeof(IrdaRc6Decoder));
    decoder->toggle = false;
    decoder->common_decoder = irda_common_decoder_alloc(&protocol_rc6);
    irda_common_set_context(decoder->common_decoder, decoder);
    return decoder;
}

IrdaMessage* irda_decoder_rc6_decode(void* decoder, bool level, uint32_t duration) {
    IrdaRc6Decoder* decoder_rc6 = decoder;
    return irda_common_decode(decoder_rc6->common_decoder, level, duration);
}

void irda_decoder_rc6_free(void* decoder) {
    IrdaRc6Decoder* decoder_rc6 = decoder;
    irda_common_decoder_free(decoder_rc6->common_decoder);
    free(decoder_rc6);
}

void irda_decoder_rc6_reset(void* decoder) {
    IrdaRc6Decoder* decoder_rc6 = decoder;
    irda_common_decoder_reset(decoder_rc6->common_decoder);
}

static DecodeStatus irda_common_decode_rc6(IrdaCommonDecoder* decoder) {
    // 4th bit lasts 2x times more
    DecodeStatus status = DecodeStatusError;
    uint16_t bit = decoder->protocol->timings.bit1_mark;
    uint16_t tolerance = decoder->protocol->timings.bit_tolerance;
    uint16_t timing = decoder->timings[0];

    bool single_timing = MATCH_BIT_TIMING(timing, bit, tolerance);
    bool double_timing = MATCH_BIT_TIMING(timing, 2*bit, tolerance);
    bool triple_timing = MATCH_BIT_TIMING(timing, 3*bit, tolerance);

    if (decoder->databit_cnt == 4) {
        furi_assert(decoder->timings_cnt == 1);
        furi_assert(decoder->avanse == true);

        if (single_timing ^ triple_timing) {
            --decoder->timings_cnt;
            ++decoder->databit_cnt;
            decoder->data[0] |= (single_timing ? !decoder->level : decoder->level) << 4;
            status = DecodeStatusOk;
        }
    } else if (decoder->databit_cnt == 5) {
        if (single_timing || triple_timing) {
            if (triple_timing)
                decoder->timings[0] = bit;
            decoder->avanse = false;
            status = irda_common_decode_manchester(decoder);
        } else if (double_timing) {
            --decoder->timings_cnt;
            status = DecodeStatusOk;
        } else {
            furi_assert(0); // tmp
        }
    } else {
        status = irda_common_decode_manchester(decoder);
    }

    return status;
}

DecodeStatus irda_common_decode_manchester(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);
    DecodeStatus status = DecodeStatusError;
    uint16_t bit = decoder->protocol->timings.bit1_mark;
    uint16_t tolerance = decoder->protocol->timings.bit_tolerance;
    uint32_t timing = decoder->timings[0];
    bool* avanse = &decoder->avanse;
    furi_assert((*avanse == true) || (*avanse == false));

    bool single_timing = MATCH_BIT_TIMING(timing, bit, tolerance);
    bool double_timing = MATCH_BIT_TIMING(timing, 2*bit, tolerance);

    if((!single_timing && !double_timing) || (double_timing && !*avanse)) {
        return DecodeStatusError;
    }

    --decoder->timings_cnt;
    status = DecodeStatusOk;

    if (*avanse == 0) {
        *avanse = 1;
    } else {
        if (single_timing)
            *avanse = 0;
    }

    if (decoder->databit_cnt < decoder->protocol->databit_len) {
        if (*avanse) {
            uint8_t index = decoder->databit_cnt / 8;
            uint8_t shift = decoder->databit_cnt % 8;   // LSB first
            if (!shift)
                decoder->data[index] = 0;
            bool inverse_level = decoder->protocol->manchester_inverse_level;
            uint8_t logic_value = inverse_level ? !decoder->level : decoder->level;
            decoder->data[index] |= (logic_value << shift);
            ++decoder->databit_cnt;
        }
        if (decoder->databit_cnt == decoder->protocol->databit_len) {
            if (decoder->level)
                status = DecodeStatusReady;
        }
    } else {
        furi_assert(decoder->level);
        /* cover case: sequence should be stopped after last bit was received */
        if (single_timing)
            status = DecodeStatusReady;
        else
            status = DecodeStatusError;
    }

    return status;
}

#if 0
DecodeStatus irda_common_decode_manchester(IrdaCommonDecoder* decoder) {
    furi_assert(decoder);
    DecodeStatus status = DecodeStatusError;
    uint16_t bit = decoder->protocol->timings.bit1_mark;
    uint16_t tolerance = decoder->protocol->timings.bit_tolerance;
    uint32_t timing = decoder->timings[0];
    bool* avanse = &decoder->avanse;
    furi_assert((*avanse == true) || (*avanse == false));

    bool single_timing = MATCH_BIT_TIMING(timing, bit, tolerance);
    bool double_timing = MATCH_BIT_TIMING(timing, 2*bit, tolerance);

    if((!single_timing && !double_timing) || (double_timing && !*avanse)) {
        furi_assert(double_timing && !*avanse);
        return DecodeStatusError;
    }

    status = DecodeStatusOk;

    if (*avanse == 0) {
        *avanse = 1;
    } else {
        if (single_timing)
            *avanse = 0;
    }

    if (*avanse) {
        uint8_t index = decoder->databit_cnt / 8;
        uint8_t shift = decoder->databit_cnt % 8;   // LSB first
        bool inverse_level = decoder->protocol->manchester_inverse_level;
        bool logic_value = inverse_level ? !!decoder->level : !decoder->level;
        decoder->data[index] |= logic_value << shift;
        ++decoder->databit_cnt;
        if (decoder->databit_cnt >= decoder->protocol->databit_len) {
            status = DecodeStatusReady;
        }
    }

    return status;
}
#endif  // 0

