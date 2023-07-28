#include "iso15693_3_i.h"

bool iso15693_3_inventory_response_parse(Iso15693_3Data* data, const BitBuffer* buf) {
    typedef struct {
        uint8_t flags;
        uint8_t dsfid;
        uint8_t uid[ISO15693_3_UID_SIZE];
    } Iso15693_3InventoryResponseLayout;

    if(bit_buffer_get_size_bytes(buf) < sizeof(Iso15693_3InventoryResponseLayout)) return false;

    const Iso15693_3InventoryResponseLayout* resp =
        (const Iso15693_3InventoryResponseLayout*)bit_buffer_get_data(buf);

    data->dsfid = resp->dsfid;

    // Reverse UID for backwards compatibility
    for(uint32_t i = 0; i < ISO15693_3_UID_SIZE; ++i) {
        data->uid[i] = resp->uid[ISO15693_3_UID_SIZE - i - 1];
    }

    return true;
}
