#include "iso14443_4a_poller_i.h"

#include <furi.h>

#include "iso14443_4a_i.h"

#define TAG "Iso14443_4aPoller"

#define ISO14443_4A_PCB_I (0x02)

Iso14443_4aError iso14443_4a_poller_process_error(Iso14443_3aError error) {
    switch(error) {
    case Iso14443_3aErrorNone:
        return Iso14443_4aErrorNone;
    case Iso14443_3aErrorNotPresent:
        return Iso14443_4aErrorNotPresent;
    case Iso14443_3aErrorColResFailed:
    case Iso14443_3aErrorCommunication:
    case Iso14443_3aErrorWrongCrc:
        return Iso14443_4aErrorProtocol;
    case Iso14443_3aErrorTimeout:
        return Iso14443_4aErrorTimeout;
    default:
        return Iso14443_4aErrorProtocol;
    }
}

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
    bit_buffer_append_byte(instance->tx_buffer, 0x80);

    Iso14443_4aError error = Iso14443_4aErrorNone;

    do {
        const Iso14443_3aError iso14443_3a_error = iso14443_3a_poller_send_standart_frame(
            instance->iso14443_3a_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            ISO14443_4A_POLLER_ATS_FWT_FC);

        if(iso14443_3a_error != Iso14443_3aErrorNone) {
            FURI_LOG_E(TAG, "ATS request failed");
            error = iso14443_4a_poller_process_error(iso14443_3a_error);
            break;

        } else if(!iso14443_4a_ats_parse(data, instance->rx_buffer)) {
            FURI_LOG_E(TAG, "Failed to parse ATS response");
            error = Iso14443_4aErrorProtocol;
            break;
        }

    } while(false);

    return error;
}

Iso14443_4aError iso14443_4a_poller_send_block(
    Iso14443_4aPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);

    const uint8_t pcb = ISO14443_4A_PCB_I | instance->protocol_state.block_number;
    instance->protocol_state.block_number ^= 1;

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_byte(instance->tx_buffer, pcb);
    bit_buffer_append(instance->tx_buffer, tx_buffer);

    Iso14443_4aError error = Iso14443_4aErrorNone;

    do {
        Iso14443_3aError iso14443_3a_error = iso14443_3a_poller_send_standart_frame(
            instance->iso14443_3a_poller, instance->tx_buffer, instance->rx_buffer, fwt);

        if(iso14443_3a_error != Iso14443_3aErrorNone) {
            error = iso14443_4a_poller_process_error(iso14443_3a_error);
            break;

        } else if(!bit_buffer_starts_with_byte(instance->rx_buffer, pcb)) {
            error = Iso14443_4aErrorProtocol;
            break;
        }

        bit_buffer_copy_right(rx_buffer, instance->rx_buffer, sizeof(pcb));
    } while(false);

    return error;
}
