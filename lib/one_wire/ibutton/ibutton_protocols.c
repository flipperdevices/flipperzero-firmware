#include "ibutton_protocols.h"

iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code) {
    iButtonProtocol protocol_id;

    for(protocol_id = 0; protocol_id < iButtonProtocolMax; ++protocol_id) {
        if(ibutton_protocols[protocol_id]->family_code == family_code) break;
    }

    return protocol_id;
}

size_t ibutton_protocols_get_data_size(iButtonProtocol protocol_id) {
    return ibutton_protocols[protocol_id]->data_size;
}

size_t ibutton_protocols_get_max_data_size() {
    size_t max_data_size = 0;

    for(size_t i = 0; i < (size_t)iButtonProtocolMax; ++i) {
        const size_t current_rom_size = ibutton_protocols[i]->data_size;
        if(current_rom_size > max_data_size) {
            max_data_size = current_rom_size;
        }
    }

    return max_data_size;
}

bool ibutton_protocols_read(OneWireHost* host, iButtonProtocolData* protocol_data, iButtonProtocol protocol_index) {
    return ibutton_protocols[protocol_index]->read(host, protocol_data);
}

void ibutton_protocols_render_data(FuriString* result, iButtonProtocolData* protocol_data, iButtonProtocol protocol_index) {
    ibutton_protocols[protocol_index]->render_data(result, protocol_data);
}

void ibutton_protocols_render_brief_data(FuriString* result, iButtonProtocolData* protocol_data, iButtonProtocol protocol_index) {
    ibutton_protocols[protocol_index]->render_brief_data(result, protocol_data);
}

