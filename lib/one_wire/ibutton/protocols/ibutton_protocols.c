#include "ibutton_protocols.h"

#include "ds1990.h"
#include "ds1992.h"

static const iButtonProtocolBase* ibutton_protocols[] = {
    &ibutton_protocol_ds1990,
    &ibutton_protocol_ds1992,
};

size_t ibutton_protocols_get_index_by_family_code(uint8_t family_code) {
    size_t protocol_index;

    for(protocol_index = 0; protocol_index < (size_t)iButtonProtocolMax; ++protocol_index) {
        if(ibutton_protocols[protocol_index]->family_code == family_code) break;
    }

    return protocol_index;
}

size_t ibutton_protocols_get_rom_size(size_t protocol_index) {
    return ibutton_protocols[protocol_index]->rom_size;
}

size_t ibutton_protocols_get_max_rom_size() {
    size_t max_rom_size = 0;

    for(size_t i = 0; i < (size_t)iButtonProtocolMax; ++i) {
        const size_t current_rom_size = ibutton_protocols[i]->rom_size;
        if(current_rom_size > max_rom_size) {
            max_rom_size = current_rom_size;
        }
    }

    return max_rom_size;
}

size_t ibutton_protocols_get_user_data_size(size_t protocol_index) {
    return ibutton_protocols[protocol_index]->user_data_size;
}

size_t ibutton_protocols_get_max_user_data_size() {
    size_t max_data_size = 0;

    for(size_t i = 0; i < (size_t)iButtonProtocolMax; ++i) {
        const size_t current_data_size = ibutton_protocols[i]->user_data_size;
        if(current_data_size > max_data_size) {
            max_data_size = current_data_size;
        }
    }

    return max_data_size;
}

bool ibutton_protocols_read_rom(OneWireHost* host, uint8_t* data, size_t protocol_index) {
    return ibutton_protocols[protocol_index]->read_rom(host, data);
}
