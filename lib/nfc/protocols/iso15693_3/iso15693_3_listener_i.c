#include "iso15693_3_listener_i.h"

#include <nfc/helpers/iso13239_crc.h>

#define TAG "Iso15693_3Listener"

static Iso15693_3Error iso15693_3_listener_inventory_handler(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        const bool afi_flag = flags & ISO15693_3_REQ_FLAG_T5_AFI_PRESENT;
        const size_t data_size_min = sizeof(uint8_t) * (afi_flag ? 2 : 1);

        if(data_size < data_size_min) {
            error = Iso15693_3ErrorFormat;
            break;
        }

        if(afi_flag) {
            const uint8_t afi = *data++;
            //TODO: Do the AFI check
            UNUSED(afi);
        }

        const uint8_t mask_len = *data++;
        const size_t data_size_required = data_size_min + mask_len;

        if(data_size != data_size_required) {
            error = Iso15693_3ErrorFormat;
            break;
        }

        if(mask_len != 0) {
            // TODO: Take mask_len and mask_value into account (if present)
        }

        bit_buffer_reset(instance->tx_buffer);

        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE); // Flags
        bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.dsfid); // DSFID
        iso15693_3_append_uid(instance->data, instance->tx_buffer); // UID

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);

    } while(false);

    return error;
}

static Iso15693_3Error iso15693_3_listener_get_system_info_handler(Iso15693_3Listener* instance) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE); // Flags

        const uint8_t system_flags = instance->data->system_info.flags;
        bit_buffer_append_byte(instance->tx_buffer, system_flags); // System info flags

        iso15693_3_append_uid(instance->data, instance->tx_buffer); // UID

        if(system_flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.dsfid);
        }
        if(system_flags & ISO15693_3_SYSINFO_FLAG_AFI) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.afi);
        }
        if(system_flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
            const uint8_t memory_info[2] = {
                instance->data->system_info.block_count - 1,
                instance->data->system_info.block_size - 1,
            };
            bit_buffer_append_bytes(instance->tx_buffer, memory_info, COUNT_OF(memory_info));
        }
        if(system_flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.ic_ref);
        }

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);

    } while(false);

    return error;
}

static Iso15693_3Error iso15693_3_listener_read_block_handler(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        typedef struct {
            uint8_t block_num;
        } Iso15693_3ReadBlockRequestLayout;

        const Iso15693_3ReadBlockRequestLayout* layout =
            (const Iso15693_3ReadBlockRequestLayout*)data;

        if(data_size != sizeof(Iso15693_3ReadBlockRequestLayout)) {
            error = Iso15693_3ErrorFormat;
            break;
        }

        if(layout->block_num >= instance->data->system_info.block_count) {
            error = Iso15693_3ErrorInternal;
            break;
        }

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE); // Flags

        if(flags & ISO15693_3_REQ_FLAG_T4_OPTION) {
            iso15693_3_append_block_security(
                instance->data,
                layout->block_num,
                instance->tx_buffer); // Block security (optional)
        }

        iso15693_3_append_block(
            instance->data, layout->block_num, instance->tx_buffer); // Block data

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);

    } while(false);

    return error;
}

static Iso15693_3Error iso15693_3_listener_write_block_handler(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {

    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        typedef struct {
            uint8_t block_num;
            uint8_t block_data[];
        } Iso15693_3WriteBlockRequestLayout;

        const Iso15693_3WriteBlockRequestLayout* layout =
            (const Iso15693_3WriteBlockRequestLayout*)data;

        if(data_size <= sizeof(Iso15693_3WriteBlockRequestLayout)) {
            error = Iso15693_3ErrorFormat;
            break;
        }

        bit_buffer_reset(instance->tx_buffer);

        const size_t block_data_size = data_size - sizeof(Iso15693_3WriteBlockRequestLayout);
        if(layout->block_num >= instance->data->system_info.block_count) {
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_ERROR);
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_ERROR_BLOCK_UNAVAILABLE);
        } else if(block_data_size != instance->data->system_info.block_size) {
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_ERROR);
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_ERROR_BLOCK_WRITE);
        } else if(iso15693_3_is_block_locked(instance->data, layout->block_num)) {
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_ERROR);
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_ERROR_BLOCK_LOCKED);
        } else {
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE);
            iso15693_3_set_block_data(instance->data, layout->block_num, layout->block_data, block_data_size);
        }

        if(flags & ISO15693_3_REQ_FLAG_T4_OPTION) {
            // FIXME: This is just a hack to emulate sending the reply after an additional EOF
            furi_delay_ms(22);
        }

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);
    } while(false);

    return error;
}

static Iso15693_3Error iso15693_3_listener_stay_quiet_handler(Iso15693_3Listener* instance) {
    instance->state = Iso15693_3ListenerStateQuiet;
    return Iso15693_3ErrorNone;
}

static Iso15693_3Error iso15693_3_listener_process_nfc_error(NfcError error) {
    Iso15693_3Error ret = Iso15693_3ErrorNone;

    if(error == NfcErrorNone) {
        ret = Iso15693_3ErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = Iso15693_3ErrorTimeout;
    } else {
        ret = Iso15693_3ErrorFieldOff;
    }

    return ret;
}

static inline Iso15693_3Error iso15693_3_listener_handle_request(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t command,
    uint8_t flags) {
    switch(command) {
    case ISO15693_3_CMD_INVENTORY:
        // An INVENTORY command is not expected here
        return Iso15693_3ErrorUnknown;
    case ISO15693_3_CMD_STAY_QUIET:
        return iso15693_3_listener_stay_quiet_handler(instance);
    case ISO15693_3_CMD_READ_BLOCK:
        return iso15693_3_listener_read_block_handler(instance, data, data_size, flags);
    case ISO15693_3_CMD_WRITE_BLOCK:
        return iso15693_3_listener_write_block_handler(instance, data, data_size, flags);
    case ISO15693_3_CMD_GET_SYS_INFO:
        return iso15693_3_listener_get_system_info_handler(instance);
    default:
        return Iso15693_3ErrorNotSupported;
    }
}

static inline Iso15693_3Error iso15693_3_listener_handle_inventory_request(
    Iso15693_3Listener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t command,
    uint8_t flags) {
    if(command == ISO15693_3_CMD_INVENTORY) {
        return iso15693_3_listener_inventory_handler(instance, data, data_size, flags);
    } else {
        // A command other than INVENTORY is not expected here
        return Iso15693_3ErrorUnknown;
    }
}

Iso15693_3Error iso15693_3_listener_ready(Iso15693_3Listener* instance) {
    furi_assert(instance);
    instance->state = Iso15693_3ListenerStateReady;
    return Iso15693_3ErrorNone;
}

Iso15693_3Error iso15693_3_listener_sleep(Iso15693_3Listener* instance) {
    furi_assert(instance);
    instance->state = Iso15693_3ListenerStateIdle;
    return Iso15693_3ErrorNone;
}

Iso15693_3Error
    iso15693_3_listener_send_frame(Iso15693_3Listener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    bit_buffer_copy(instance->tx_buffer, tx_buffer);
    iso13239_crc_append(Iso13239CrcTypeDefault, instance->tx_buffer);

    NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
    return iso15693_3_listener_process_nfc_error(error);
}

Iso15693_3Error
    iso15693_3_listener_process_request(Iso15693_3Listener* instance, const BitBuffer* rx_buffer) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        typedef struct {
            uint8_t flags;
            uint8_t command;
            uint8_t data[];
        } Iso15693_3RequestLayout;

        const size_t buf_size = bit_buffer_get_size_bytes(rx_buffer);
        const size_t buf_size_min = sizeof(Iso15693_3RequestLayout);

        if(buf_size < buf_size_min) {
            error = Iso15693_3ErrorFormat;
            break;
        }

        const Iso15693_3RequestLayout* layout =
            (const Iso15693_3RequestLayout*)bit_buffer_get_data(rx_buffer);

        const bool inventory_flag = layout->flags & ISO15693_3_REQ_FLAG_INVENTORY_T5;

        if(!inventory_flag) {
            const bool selected_mode = layout->flags & ISO15693_3_REQ_FLAG_T4_SELECTED;
            const bool addressed_mode = layout->flags & ISO15693_3_REQ_FLAG_T4_ADDRESSED;

            if(instance->state == Iso15693_3ListenerStateSelected) {
                // If the card is not selected, ignore the command
                if(!selected_mode) break;
            } else if(instance->state == Iso15693_3ListenerStateQuiet) {
                // If the card is quiet, ignore non-addressed commands
                if(!addressed_mode) break;
            }

            const uint8_t* data;
            size_t data_size;

            if(addressed_mode) {
                // In addressed mode, UID must be included in each command
                const size_t buf_size_min_addr = buf_size_min + ISO15693_3_UID_SIZE;

                if(buf_size < buf_size_min_addr) {
                    error = Iso15693_3ErrorFormat;
                    break;
                } else if(!iso15693_3_is_equal_uid(instance->data, layout->data)) {
                    // In addressed mode, ignore all commands with non-matching UID
                    break;
                }

                data = &layout->data[ISO15693_3_UID_SIZE];
                data_size = buf_size - buf_size_min_addr;

            } else {
                data = layout->data;
                data_size = buf_size - buf_size_min;
            }

            error = iso15693_3_listener_handle_request(
                instance, data, data_size, layout->command, layout->flags);

        } else {
            // If the card is quiet, ignore inventory commands
            if(instance->state == Iso15693_3ListenerStateQuiet) {
                break;
            }

            // Special case handler when inventory flag is set
            error = iso15693_3_listener_handle_inventory_request(
                instance, layout->data, buf_size - buf_size_min, layout->command, layout->flags);
        }

    } while(false);

    return error;
}
