#include "iso14443_4b_poller_i.h"

#include <furi.h>

#include "iso14443_4b_i.h"

#define TAG "Iso14443_4bPoller"

Iso14443_4bError iso14443_4b_poller_halt(Iso14443_4bPoller* instance) {
    furi_assert(instance);

    iso14443_3b_poller_halt(instance->iso14443_3b_poller);
    instance->poller_state = Iso14443_4bPollerStateIdle;

    return Iso14443_4bErrorNone;
}

Iso14443_4bError
    iso14443_4b_poller_async_read_ats(Iso14443_4bPoller* instance, SimpleArray* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_byte(instance->tx_buffer, ISO14443_4B_CMD_READ_ATS);
    bit_buffer_append_byte(instance->tx_buffer, ISO14443_4B_FSDI_256 << 4);

    Iso14443_4bError error = Iso14443_4bErrorNone;

    do {
        const Iso14443_3bError iso14443_3b_error = iso14443_3b_poller_send_frame(
            instance->iso14443_3b_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            ISO14443_4B_POLLER_ATS_FWT_FC);

        if(iso14443_3b_error != Iso14443_3bErrorNone) {
            FURI_LOG_E(TAG, "ATS request failed");
            error = iso14443_4b_process_error(iso14443_3b_error);
            break;

        } else if(!iso14443_4b_ats_parse(data, instance->rx_buffer)) {
            FURI_LOG_E(TAG, "Failed to parse ATS response");
            error = Iso14443_4bErrorProtocol;
            break;
        }

    } while(false);

    return error;
}

Iso14443_4bError iso14443_4b_poller_send_block(
    Iso14443_4bPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);

    const uint8_t pcb = ISO14443_4B_BLOCK_PCB_I | ISO14443_4B_BLOCK_PCB |
                        instance->protocol_state.block_number;
    instance->protocol_state.block_number ^= 1;

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_byte(instance->tx_buffer, pcb);
    bit_buffer_append(instance->tx_buffer, tx_buffer);

    Iso14443_4bError error = Iso14443_4bErrorNone;

    do {
        Iso14443_3bError iso14443_3b_error = iso14443_3b_poller_send_frame(
            instance->iso14443_3b_poller, instance->tx_buffer, instance->rx_buffer, fwt);

        if(iso14443_3b_error != Iso14443_3bErrorNone) {
            error = iso14443_4b_process_error(iso14443_3b_error);
            break;

        } else if(!bit_buffer_starts_with_byte(instance->rx_buffer, pcb)) {
            error = Iso14443_4bErrorProtocol;
            break;
        }

        bit_buffer_copy_right(rx_buffer, instance->rx_buffer, sizeof(pcb));
    } while(false);

    return error;
}
