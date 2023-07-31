#include "iso15693_3_i.h"

bool iso15693_3_inventory_response_parse(Iso15693_3Data* data, const BitBuffer* buf) {
    typedef struct {
        uint8_t flags;
        uint8_t dsfid;
        uint8_t uid[ISO15693_3_UID_SIZE];
    } InventoryResponseLayout;

    if(bit_buffer_get_size_bytes(buf) < sizeof(InventoryResponseLayout)) return false;

    const InventoryResponseLayout* resp = (const InventoryResponseLayout*)bit_buffer_get_data(buf);

    data->dsfid = resp->dsfid;

    // Reverse UID for backwards compatibility
    for(uint32_t i = 0; i < ISO15693_3_UID_SIZE; ++i) {
        data->uid[i] = resp->uid[ISO15693_3_UID_SIZE - i - 1];
    }

    return true;
}

bool iso15693_3_system_info_response_parse(Iso15693_3Data* data, const BitBuffer* buf) {
    typedef struct {
        uint8_t flags;
        uint8_t info_flags;
        uint8_t uid[ISO15693_3_UID_SIZE];
        uint8_t extra[];
    } SystemInfoResponseLayout;

    if(bit_buffer_get_size_bytes(buf) < sizeof(SystemInfoResponseLayout)) return false;

    const SystemInfoResponseLayout* resp =
        (const SystemInfoResponseLayout*)bit_buffer_get_data(buf);

    const uint8_t* extra = resp->extra;

    if(resp->info_flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
        data->dsfid = *extra++;
    }

    if(resp->info_flags & ISO15693_3_SYSINFO_FLAG_AFI) {
        data->afi = *extra++;
    }

    if(resp->info_flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
        // For some reason, we need to add 1 to these values
        data->block_count = *extra++ + 1;
        data->block_size = (*extra++ & 0x1F) + 1;
    }

    if(resp->info_flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
        data->ic_ref = *extra;
    }

    return true;
}

bool iso15693_3_read_block_response_parse(uint8_t* data, uint8_t block_size, const BitBuffer* buf) {
    typedef struct {
        uint8_t flags;
        uint8_t block_data[];
    } ReadBlockResponseLayout;

    bool parsed = false;

    do {
        const size_t buf_size = bit_buffer_get_size_bytes(buf);
        if(buf_size <= sizeof(ReadBlockResponseLayout)) return false;

        const ReadBlockResponseLayout* resp =
            (const ReadBlockResponseLayout*)bit_buffer_get_data(buf);

        if(resp->flags & ISO15693_3_RESP_FLAG_ERROR) break;

        const size_t received_block_size = buf_size - sizeof(ReadBlockResponseLayout);
        if(received_block_size != block_size) break;

        memcpy(data, resp->block_data, received_block_size);
        parsed = true;
    } while(false);

    return parsed;
}

bool iso15693_3_get_block_security_response_parse(
    uint8_t* data,
    uint16_t block_count,
    const BitBuffer* buf) {
    typedef struct {
        uint8_t flags;
        uint8_t block_security[];
    } GetBlockSecurityResponseLayout;

    bool parsed = false;

    do {
        const size_t buf_size = bit_buffer_get_size_bytes(buf);
        if(buf_size <= sizeof(GetBlockSecurityResponseLayout)) return false;

        const GetBlockSecurityResponseLayout* resp =
            (const GetBlockSecurityResponseLayout*)bit_buffer_get_data(buf);

        if(resp->flags & ISO15693_3_RESP_FLAG_ERROR) break;

        const size_t received_block_count = buf_size - sizeof(GetBlockSecurityResponseLayout);
        if(received_block_count != block_count) break;

        memcpy(data, resp->block_security, received_block_count);
        parsed = true;
    } while(false);

    return parsed;
}
