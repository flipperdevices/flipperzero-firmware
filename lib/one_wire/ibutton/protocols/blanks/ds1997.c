#include "ds1971.h"

#include <core/kernel.h>

#define DS1971_CMD_READ_SCRATCHPAD 0xAA
#define DS1971_CMD_COPY_SCRATCHPAD 0x55
#define DS1971_CMD_WRITE_SCRATCHPAD 0x0F
#define DS1971_CMD_FINALIZATION 0xA5

bool ds1971_write(OneWireHost* host, const uint8_t* data, size_t data_size) {
    onewire_host_reset(host);
    onewire_host_skip(host);
    // Starting writing from address 0x0000
    onewire_host_write(host, DS1971_CMD_WRITE_SCRATCHPAD);
    onewire_host_write(host, 0x00);
    // Write data to scratchpad
    onewire_host_write_bytes(host, data, data_size);
    furi_delay_ms(250);

    // Read data from scratchpad and verify
    bool pad_valid = false;
    if(onewire_host_reset(host)) {
        pad_valid = true;
        onewire_host_skip(host);
        onewire_host_write(host, DS1971_CMD_READ_SCRATCHPAD);
        onewire_host_write(host, 0x00);

        for(size_t i = 0; i < data_size; ++i) {
            uint8_t scratch = onewire_host_read(host);
            if(data[i] != scratch) {
                pad_valid = false;
                break;
            }
        }
    }
    furi_delay_ms(250);

    // Copy scratchpad to memory and confirm
    if(pad_valid) {
        if(onewire_host_reset(host)) {
            onewire_host_skip(host);
            onewire_host_write(host, DS1971_CMD_COPY_SCRATCHPAD);
            onewire_host_write(host, DS1971_CMD_FINALIZATION);
            furi_delay_ms(250);
        }
    }

    // TODO: Better error handling
    return pad_valid;
}