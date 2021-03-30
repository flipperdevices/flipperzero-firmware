#pragma once
#include "decoder-analyzer.h"
#include "decoder-emmarine.h"
#include "decoder-hid26.h"
#include "decoder-indala.h"

class RfidReader {
public:
    RfidReader();
    void start();
    void stop();
    bool read();

private:
    friend struct RfidReaderAccessor;

    //DecoderAnalyzer decoder_analyzer;
    DecoderEMMarine decoder_em;
    DecoderHID26 decoder_hid26;
    DecoderIndala decoder_indala;

    uint32_t last_dwt_value;

    void start_comparator(void);
    void decode(bool polarity);
};
