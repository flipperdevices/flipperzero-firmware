#include "decoder-analyzer.h"
#include <furi.h>
#include <api-hal.h>

bool DecoderAnalyzer::read(uint8_t* _data, uint8_t _data_size) {
    bool result = false;

    if(ready) {
        result = true;

        for(size_t i = 0; i < data_size; i++) {
            printf("%lu ", data[i]);
            if((i + 1) % 8 == 0) printf("\r\n");
        }
        printf("\r\n--------\r\n");

        ready = false;
    }

    return result;
}

void DecoderAnalyzer::process_front(bool polarity, uint32_t time) {
    if(ready) return;

    if(polarity) {
        gpio_write(&ext_pa7_gpio, 1);
    } else {
        gpio_write(&ext_pa7_gpio, 0);
    }

    data[data_index] = time;

    if(data_index < data_size) {
        data_index++;
    } else {
        data_index = 0;
        ready = true;
    }
}

DecoderAnalyzer::DecoderAnalyzer() {
    data = reinterpret_cast<uint32_t*>(calloc(data_size, sizeof(uint32_t)));
    furi_check(data);
    data_index = 0;
    ready = false;
}

DecoderAnalyzer::~DecoderAnalyzer() {
    free(data);
}

void DecoderAnalyzer::reset_state() {
}
