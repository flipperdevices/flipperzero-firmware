#include "ibutton_protocols.h"

iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code) {
    iButtonProtocol protocol_id;

    for(protocol_id = 0; protocol_id < iButtonProtocolMax; ++protocol_id) {
        if(ibutton_protocols[protocol_id]->family_code == family_code) break;
    }

    return protocol_id;
}

iButtonProtocol ibutton_protocols_get_id_by_name(const char* protocol_name) {
    iButtonProtocol protocol_id;

    for(protocol_id = 0; protocol_id < iButtonProtocolMax; ++protocol_id) {
        if(!strcmp(ibutton_protocols[protocol_id]->name, protocol_name)) break;
    }

    return protocol_id;
}

uint32_t ibutton_protocols_get_features(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->features;
}

size_t ibutton_protocols_get_data_size(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
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

const char* ibutton_protocols_get_manufacturer(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->manufacturer;
}

const char* ibutton_protocols_get_name(iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->name;
}

bool ibutton_protocols_read(OneWireHost* host, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->read(host, protocol_data);
}

bool ibutton_protocols_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->save(ff, protocol_data);
}

bool ibutton_protocols_load(FlipperFormat* ff, uint32_t version, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->load(ff, version, protocol_data);
}

void ibutton_protocols_render_data(FuriString* result, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->render_data(result, protocol_data);
}

void ibutton_protocols_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    ibutton_protocols[protocol_id]->render_brief_data(result, protocol_data);
}

bool ibutton_protocols_is_valid(const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id) {
    furi_assert(protocol_id < iButtonProtocolMax);
    return ibutton_protocols[protocol_id]->is_valid(protocol_data);
}

