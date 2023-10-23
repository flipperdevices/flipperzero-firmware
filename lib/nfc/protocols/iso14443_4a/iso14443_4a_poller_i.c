#include "iso14443_4a_poller_i.h"

#include <furi.h>

#include "iso14443_4a_i.h"

#define TAG "Iso14443_4aPoller"

#define ISO14443_4A_FSDI_256 (0x8U)

Iso14443_4aError iso14443_4a_poller_halt(Iso14443_4aPoller* instance) {
    furi_assert(instance);

    iso14443_3a_poller_halt(instance->iso14443_3a_poller);
    instance->poller_state = Iso14443_4aPollerStateIdle;

    return Iso14443_4aErrorNone;
}

Iso14443_4aError
    iso14443_4a_poller_async_read_ats(Iso14443_4aPoller* instance, Iso14443_4aAtsData* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_byte(instance->tx_buffer, ISO14443_4A_CMD_READ_ATS);
    bit_buffer_append_byte(instance->tx_buffer, ISO14443_4A_FSDI_256 << 4);

    Iso14443_4aError error = Iso14443_4aErrorNone;

    do {
        const Iso14443_3aError iso14443_3a_error = iso14443_3a_poller_send_standard_frame(
            instance->iso14443_3a_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            ISO14443_4A_POLLER_ATS_FWT_FC);

        if(iso14443_3a_error != Iso14443_3aErrorNone) {
            FURI_LOG_E(TAG, "ATS request failed");
            error = iso14443_4a_process_error(iso14443_3a_error);
            break;

        } else if(!iso14443_4a_ats_parse(data, instance->rx_buffer)) {
            FURI_LOG_E(TAG, "Failed to parse ATS response");
            error = Iso14443_4aErrorProtocol;
            break;
        }

    } while(false);

    return error;
}

static void iso14443_4a_dump_buffer(const BitBuffer* buf) {
    const size_t buf_size = bit_buffer_get_size_bytes(buf);
    FuriString* tmp = furi_string_alloc_printf("ISO14443-3A frame (%zu bytes):", buf_size);

    for(size_t i = 0; i < buf_size; ++i) {
        furi_string_cat_printf(tmp, " %02X", bit_buffer_get_byte(buf, i));
    }

    FURI_LOG_D(TAG, "%s", furi_string_get_cstr(tmp));

    furi_string_free(tmp);
}

Iso14443_4aError iso14443_4a_poller_send_block(
    Iso14443_4aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    iso14443_4_layer_encode_block(instance->iso14443_4_layer, tx_buffer, instance->tx_buffer);

    Iso14443_4aError error = Iso14443_4aErrorNone;

    do {
        Iso14443_3aError iso14443_3a_error = iso14443_3a_poller_send_standard_frame(
            instance->iso14443_3a_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            iso14443_4a_get_fwt_fc_max(instance->data));

        iso14443_4a_dump_buffer(instance->rx_buffer);

        if(iso14443_3a_error != Iso14443_3aErrorNone) {
            FURI_LOG_D(TAG, "ISO14443-3A frame failure: error %d", iso14443_3a_error);
            FURI_LOG_D(TAG, "FWT was: %lu", iso14443_4a_get_fwt_fc_max(instance->data));
            error = iso14443_4a_process_error(iso14443_3a_error);
            break;

        } else if(!iso14443_4_layer_decode_block(
                      instance->iso14443_4_layer, rx_buffer, instance->rx_buffer)) {
            FURI_LOG_D(TAG, "ISO14443-4 block decode failure");
            iso14443_4a_dump_buffer(instance->rx_buffer);
            error = Iso14443_4aErrorProtocol;
            break;
        }
    } while(false);

    return error;
}
