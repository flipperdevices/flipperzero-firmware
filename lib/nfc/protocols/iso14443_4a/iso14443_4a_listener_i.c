#include "iso14443_4a_listener_i.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>

Iso14443_4aError
    iso14443_4a_listener_send_ats(Iso14443_4aListener* instance, const SimpleArray* data) {
    bit_buffer_copy_bytes(
        instance->tx_buffer, simple_array_cget_data(data), simple_array_get_count(data));

    const Iso14443_3aError error = iso14443_3a_listener_send_standard_frame(
        instance->iso14443_3a_listener, instance->tx_buffer);
    return iso14443_4a_process_error(error);
}
