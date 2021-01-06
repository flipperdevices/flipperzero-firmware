#include "irda-decoder.h"

IrDADecoder* alloc_decoder(void) {
    IrDADecoder* decoder = malloc(sizeof(IrDADecoder));

    // init
    decoder->nec.state = WAIT_PREAMBULA_HIGH;

    return decoder;
}

void free_decoder(IrDADecoder* decoder) {
    free(decoder);
}

bool process_decoder(
    IrDADecoder* decoder,
    bool start_polarity,
    uint32_t* timings,
    uint32_t timings_length) {
    // process data
    for(uint32_t timings_index = 0; timings_index < timings_length; timings_index++) {
        if(process_decoder_nec(&decoder->nec, start_polarity, timings[timings_index])) {
        }

        start_polarity = !start_polarity;
    }
}