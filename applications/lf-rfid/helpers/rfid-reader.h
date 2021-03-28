#pragma once
#include "decoder-emmarine.h"
#include "decoder-hid26.h"

class RfidReader {
public:
    RfidReader();
    void start();
    void stop();
    bool read();

private:
    friend struct RfidReaderAccessor;

    DecoderEMMarine decoder_em;
    DecoderHID26 decoder_hid26;

    uint32_t last_dwt_value;

    void start_comparator(void);
    void decode(bool polarity);
};
