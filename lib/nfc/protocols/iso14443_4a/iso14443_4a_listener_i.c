#include "iso14443_4a_listener_i.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>

Iso14443_4aError
    iso14443_4a_listener_send_ats(Iso14443_4aListener* instance, const Iso14443_4aAtsData* data) {
    bit_buffer_copy_bytes(instance->tx_buffer, (const uint8_t*)data, sizeof(Iso14443_4aAtsData));
    const Iso14443_3aError error = iso14443_3a_listener_send_standard_frame(instance->iso14443_3a_listener, instance->tx_buffer);
    return iso14443_4a_process_error(error);
}
