#include "picopass_i.h"
#include "picopass_listener_i.h"
#include "picopass_keys.h"

#include <furi/furi.h>

#define PICOPASS_LISTENER_HAS_MASK(x, b) ((x & b) == b)

typedef enum {
    PicopassListenerCommandProcessed,
    PicopassListenerCommandSilent,
    PicopassListenerCommandSendSoF,
    PicopassListenerCommandStop,
} PicopassListenerCommand;

typedef PicopassListenerCommand (
    *PicopassListenerCommandHandler)(PicopassListener* instance, BitBuffer* buf);

typedef struct {
    uint8_t start_byte_cmd;
    size_t cmd_len_bits;
    PicopassListenerCommandHandler handler;
} PicopassListenerCmd;

// CSNs from Proxmark3 repo
static const uint8_t loclass_csns[LOCLASS_NUM_CSNS][PICOPASS_BLOCK_LEN] = {
    {0x01, 0x0A, 0x0F, 0xFF, 0xF7, 0xFF, 0x12, 0xE0},
    {0x0C, 0x06, 0x0C, 0xFE, 0xF7, 0xFF, 0x12, 0xE0},
    {0x10, 0x97, 0x83, 0x7B, 0xF7, 0xFF, 0x12, 0xE0},
    {0x13, 0x97, 0x82, 0x7A, 0xF7, 0xFF, 0x12, 0xE0},
    {0x07, 0x0E, 0x0D, 0xF9, 0xF7, 0xFF, 0x12, 0xE0},
    {0x14, 0x96, 0x84, 0x76, 0xF7, 0xFF, 0x12, 0xE0},
    {0x17, 0x96, 0x85, 0x71, 0xF7, 0xFF, 0x12, 0xE0},
    {0xCE, 0xC5, 0x0F, 0x77, 0xF7, 0xFF, 0x12, 0xE0},
    {0xD2, 0x5A, 0x82, 0xF8, 0xF7, 0xFF, 0x12, 0xE0},
};

static void picopass_listener_reset(PicopassListener* instance) {
    instance->state = PicopassListenerStateIdle;
}

static void picopass_listener_loclass_update_csn(PicopassListener* instance) {
    // collect LOCLASS_NUM_PER_CSN nonces in a row for each CSN
    const uint8_t* csn =
        loclass_csns[(instance->key_block_num / LOCLASS_NUM_PER_CSN) % LOCLASS_NUM_CSNS];
    memcpy(instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data, csn, sizeof(PicopassBlock));

    uint8_t key[PICOPASS_BLOCK_LEN] = {};
    loclass_iclass_calc_div_key(csn, picopass_iclass_key, key, false);
    memcpy(instance->data->AA1[PICOPASS_SECURE_KD_BLOCK_INDEX].data, key, sizeof(PicopassBlock));

    picopass_listener_init_cipher_state_key(instance, key);
}

PicopassListenerCommand
    picopass_listener_actall_handler(PicopassListener* instance, BitBuffer* buf) {
    UNUSED(buf);

    if(instance->state != PicopassListenerStateHalt) {
        instance->state = PicopassListenerStateActive;
    }
    // nfc_set_fdt_listen_fc(instance->nfc, 1000);

    return PicopassListenerCommandSendSoF;
}

PicopassListenerCommand picopass_listener_act_handler(PicopassListener* instance, BitBuffer* buf) {
    UNUSED(buf);

    PicopassListenerCommand command = PicopassListenerCommandSendSoF;

    if(instance->state != PicopassListenerStateActive) {
        command = PicopassListenerCommandSilent;
    }

    return command;
}

PicopassListenerCommand
    picopass_listener_halt_handler(PicopassListener* instance, BitBuffer* buf) {
    UNUSED(buf);

    PicopassListenerCommand command = PicopassListenerCommandSendSoF;

    // Technically we should go to StateHalt, but since we can't detect the field dropping we drop to idle instead
    instance->state = PicopassListenerStateIdle;

    return command;
}

PicopassListenerCommand
    picopass_listener_identify_handler(PicopassListener* instance, BitBuffer* buf) {
    UNUSED(buf);

    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if(instance->state != PicopassListenerStateActive) break;
        picopass_listener_write_anticoll_csn(instance, instance->tx_buffer);
        PicopassError error = picopass_listener_send_frame(instance, instance->tx_buffer);
        if(error != PicopassErrorNone) {
            FURI_LOG_D(TAG, "Error sending CSN: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_select_handler(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if((instance->state == PicopassListenerStateHalt) ||
           (instance->state == PicopassListenerStateIdle)) {
            bit_buffer_copy_bytes(
                instance->tmp_buffer,
                instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data,
                sizeof(PicopassBlock));
        } else {
            picopass_listener_write_anticoll_csn(instance, instance->tmp_buffer);
        }
        const uint8_t* listener_uid = bit_buffer_get_data(instance->tmp_buffer);
        const uint8_t* received_data = bit_buffer_get_data(buf);

        if(memcmp(listener_uid, &received_data[1], PICOPASS_BLOCK_LEN) != 0) {
            if(instance->state == PicopassListenerStateActive) {
                instance->state = PicopassListenerStateIdle;
            } else if(instance->state == PicopassListenerStateSelected) {
                // Technically we should go to StateHalt, but since we can't detect the field dropping we drop to idle instead
                instance->state = PicopassListenerStateIdle;
            }
            break;
        }

        instance->state = PicopassListenerStateSelected;
        bit_buffer_copy_bytes(
            instance->tx_buffer,
            instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data,
            sizeof(PicopassBlock));

        PicopassError error = picopass_listener_send_frame(instance, instance->tx_buffer);
        if(error != PicopassErrorNone) {
            FURI_LOG_D(TAG, "Error sending select response: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_read_handler(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        uint8_t block_num = bit_buffer_get_byte(buf, 1);
        if(block_num > PICOPASS_MAX_APP_LIMIT) break;

        bool secured = (instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] &
                        PICOPASS_FUSE_CRYPT10) != PICOPASS_FUSE_CRYPT0;

        // TODO: Check CRC?
        // TODO: Check auth?

        bit_buffer_reset(instance->tx_buffer);
        if(secured && ((block_num == PICOPASS_SECURE_KD_BLOCK_INDEX) ||
                       (block_num == PICOPASS_SECURE_KC_BLOCK_INDEX))) {
            for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
                bit_buffer_append_byte(instance->tx_buffer, 0xff);
            }
        } else {
            bit_buffer_copy_bytes(
                instance->tx_buffer, instance->data->AA1[block_num].data, sizeof(PicopassBlock));
        }
        PicopassError error = picopass_listener_send_frame(instance, instance->tx_buffer);
        if(error != PicopassErrorNone) {
            FURI_LOG_D(TAG, "Failed to tx read block response: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

static PicopassListenerCommand
    picopass_listener_readcheck(PicopassListener* instance, BitBuffer* buf, uint8_t key_block_num) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if(instance->state != PicopassListenerStateSelected) break;
        uint8_t block_num = bit_buffer_get_byte(buf, 1);
        if(block_num != PICOPASS_SECURE_EPURSE_BLOCK_INDEX) break;

        // note that even non-secure chips seem to reply to READCHECK still

        // loclass mode doesn't do any card side crypto, just logs the readers crypto, so no-op in this mode
        // we can also no-op if the key block is the same, CHECK re-inits if it failed already
        if((instance->key_block_num != key_block_num) &&
           (instance->mode != PicopassListenerModeLoclass)) {
            instance->key_block_num = key_block_num;
            picopass_listener_init_cipher_state(instance);
        }

        // DATA(8)
        bit_buffer_copy_bytes(
            instance->tx_buffer, instance->data->AA1[block_num].data, sizeof(PicopassBlock));
        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        if(error != NfcErrorNone) {
            FURI_LOG_D(TAG, "Failed to tx read check response: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_readcheck_kd_handler(PicopassListener* instance, BitBuffer* buf) {
    return picopass_listener_readcheck(instance, buf, PICOPASS_SECURE_KD_BLOCK_INDEX);
}

PicopassListenerCommand
    picopass_listener_readcheck_kc_handler(PicopassListener* instance, BitBuffer* buf) {
    return picopass_listener_readcheck(instance, buf, PICOPASS_SECURE_KC_BLOCK_INDEX);
}

PicopassListenerCommand
    picopass_listener_check_handler_loclass(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;
    NfcCommand callback_command = NfcCommandContinue;

    // LOCLASS Reader attack mode
    do {
#ifndef PICOPASS_DEBUG_IGNORE_LOCLASS_STD_KEY
        // loclass mode stores the derived standard debit key in Kd to check

        PicopassBlock key = instance->data->AA1[PICOPASS_SECURE_KD_BLOCK_INDEX];
        uint8_t rmac[4];
        const uint8_t* rx_data = bit_buffer_get_data(buf);
        loclass_opt_doReaderMAC_2(instance->cipher_state, &rx_data[1], rmac, key.data);

        if(!memcmp(&rx_data[5], rmac, 4)) {
            // MAC from reader matches Standard Key, keyroll mode or non-elite keyed reader.
            // Either way no point logging it.

            FURI_LOG_W(TAG, "loclass: standard key detected during collection");
            if(instance->callback) {
                instance->event.type = PicopassListenerEventTypeLoclassGotStandardKey;
                callback_command = instance->callback(instance->event, instance->context);
                if(callback_command == NfcCommandStop) {
                    command = PicopassListenerCommandStop;
                }
            }

            // Don't reset the state as the reader may try a different key next without going through anticoll
            // The reader is always free to redo the anticoll if it wants to anyway

            break;
        }
#endif

        // Save to buffer to defer flushing when we rotate CSN
        memcpy(
            instance->loclass_mac_buffer + ((instance->key_block_num % LOCLASS_NUM_PER_CSN) * 8),
            &rx_data[1],
            8);

        // Rotate to the next CSN/attempt
        instance->key_block_num++;

        // CSN changed
        if(instance->key_block_num % LOCLASS_NUM_PER_CSN == 0) {
            // Flush NR-MACs for this CSN to SD card
            for(int i = 0; i < LOCLASS_NUM_PER_CSN; i++) {
                loclass_writer_write_params(
                    instance->writer,
                    instance->key_block_num + i - LOCLASS_NUM_PER_CSN,
                    instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data,
                    instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data,
                    instance->loclass_mac_buffer + (i * 8),
                    instance->loclass_mac_buffer + (i * 8) + 4);
            }

            if(instance->key_block_num < LOCLASS_NUM_CSNS * LOCLASS_NUM_PER_CSN) {
                picopass_listener_loclass_update_csn(instance);
                // Only reset the state when we change to a new CSN for the same reason as when we get a standard key
                instance->state = PicopassListenerStateIdle;
            }
        }
        if(instance->callback) {
            instance->event.type = PicopassListenerEventTypeLoclassGotMac;
            instance->callback(instance->event, instance->context);
        }

    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_save_mac(PicopassListener* instance, const uint8_t* rx_data) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;
    Picopass* picopass = instance->context;

    PicopassDevice* dev = picopass->dev;

    const uint8_t* csn = instance->data->AA1[PICOPASS_CSN_BLOCK_INDEX].data;
    const uint8_t* epurse = instance->data->AA1[PICOPASS_SECURE_EPURSE_BLOCK_INDEX].data;

    FuriString* temp_str = furi_string_alloc();
    FuriString* filename = furi_string_alloc();
    FlipperFormat* file = flipper_format_file_alloc(dev->storage);

    for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
        furi_string_cat_printf(filename, "%02x", csn[i]);
    }
    furi_string_cat_printf(filename, "_");
    for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
        furi_string_cat_printf(filename, "%02x", epurse[i]);
    }

    furi_string_printf(
        temp_str, "%s/%s%s", STORAGE_APP_DATA_PATH_PREFIX, furi_string_get_cstr(filename), ".mac");
    do {
        // Open file
        if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) break;

        if(!flipper_format_write_hex(file, "NR-MAC", rx_data + 1, PICOPASS_BLOCK_LEN)) break;

        FURI_LOG_D(
            TAG,
            "Saved nr-mac: %02x %02x %02x %02x %02x %02x %02x %02x",
            // Skip command byte [0]
            rx_data[1],
            rx_data[2],
            rx_data[3],
            rx_data[4],
            rx_data[5],
            rx_data[6],
            rx_data[7],
            rx_data[8]);

        notification_message(picopass->notifications, &sequence_double_vibro);
        command = PicopassListenerCommandStop;
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventNrMacSaved);
    } while(0);

    furi_string_free(temp_str);
    furi_string_free(filename);
    flipper_format_free(file);

    return command;
}

PicopassListenerCommand
    picopass_listener_check_handler_emulation(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        bool secured = (instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] &
                        PICOPASS_FUSE_CRYPT10) != PICOPASS_FUSE_CRYPT0;
        if(!secured) break;

        uint8_t rmac[4] = {};
        uint8_t tmac[4] = {};
        const uint8_t* key = instance->data->AA1[instance->key_block_num].data;
        bool no_key = picopass_is_memset(key, 0x00, PICOPASS_BLOCK_LEN);
        const uint8_t* rx_data = bit_buffer_get_data(buf);

        if(no_key) {
            // We're emulating a partial dump of an iClass SE card and should capture the NR and MAC
            command = picopass_listener_save_mac(instance, rx_data);
            break;
        } else {
            loclass_opt_doBothMAC_2(instance->cipher_state, &rx_data[1], rmac, tmac, key);

#ifndef PICOPASS_DEBUG_IGNORE_BAD_RMAC
            if(memcmp(&rx_data[5], rmac, PICOPASS_MAC_LEN)) {
                // Bad MAC from reader, do not send a response.
                FURI_LOG_I(TAG, "Got bad MAC from reader");
                // Reset the cipher state since we don't do it in READCHECK
                picopass_listener_init_cipher_state(instance);
                break;
            }
#endif

            bit_buffer_copy_bytes(instance->tx_buffer, tmac, sizeof(tmac));
            NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
            if(error != NfcErrorNone) {
                FURI_LOG_D(TAG, "Failed tx update response: %d", error);
                break;
            }
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}
PicopassListenerCommand
    picopass_listener_check_handler(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if(instance->state != PicopassListenerStateSelected) break;
        if(instance->mode == PicopassListenerModeLoclass) {
            command = picopass_listener_check_handler_loclass(instance, buf);
        } else {
            command = picopass_listener_check_handler_emulation(instance, buf);
        }
    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_update_handler(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if(instance->mode == PicopassListenerModeLoclass) break;
        if(instance->state != PicopassListenerStateSelected) break;

        PicopassBlock config_block = instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX];
        bool pers_mode = PICOPASS_LISTENER_HAS_MASK(config_block.data[7], PICOPASS_FUSE_PERS);
        bool secured = (instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] &
                        PICOPASS_FUSE_CRYPT10) != PICOPASS_FUSE_CRYPT0;

        const uint8_t* rx_data = bit_buffer_get_data(buf);
        uint8_t block_num = rx_data[1];
        if(block_num == PICOPASS_CSN_BLOCK_INDEX) break; // CSN is always read only
        if(!pers_mode && PICOPASS_LISTENER_HAS_MASK(config_block.data[3], 0x80))
            break; // Chip is in RO mode, no updated possible (even ePurse)
        if(!pers_mode && ((secured && block_num == PICOPASS_SECURE_AIA_BLOCK_INDEX) ||
                          (!secured && block_num == PICOPASS_NONSECURE_AIA_BLOCK_INDEX)))
            break; // AIA can only be set in personalisation mode
        if(!pers_mode && secured &&
           ((block_num == PICOPASS_SECURE_KD_BLOCK_INDEX ||
             block_num == PICOPASS_SECURE_KC_BLOCK_INDEX) &&
            (!PICOPASS_LISTENER_HAS_MASK(config_block.data[7], PICOPASS_FUSE_CRYPT10))))
            break; // TODO: Is this the right response?

        if(block_num >= 6 && block_num <= 12) {
            // bit0 is block6, up to bit6 being block12
            if(!PICOPASS_LISTENER_HAS_MASK(config_block.data[3], (1 << (block_num - 6)))) {
                // Block is marked as read-only, deny writing
                break; // TODO: Is this the right response?
            }
        }

        // TODO: Check CRC/SIGN depending on if in secure mode
        // Check correct key
        // -> Kd only allows decrementing e-Purse
        // -> per-app controlled by key access config
        // bool keyAccess = PICOPASS_LISTENER_HAS_MASK(config_block.data[5], 0x01);
        // -> must auth with that key to change it

        PicopassBlock new_block = {};
        switch(block_num) {
        case PICOPASS_CONFIG_BLOCK_INDEX:
            new_block.data[0] = config_block.data[0]; // Applications Limit
            new_block.data[1] = config_block.data[1] & rx_data[3]; // OTP
            new_block.data[2] = config_block.data[2] & rx_data[4]; // OTP
            new_block.data[3] = config_block.data[3] & rx_data[5]; // Block Write Lock
            new_block.data[4] = config_block.data[4]; // Chip Config
            new_block.data[5] = config_block.data[5]; // Memory Config
            new_block.data[6] = rx_data[8]; // EAS
            new_block.data[7] = config_block.data[7]; // Fuses

            // Some parts allow w (but not e) if in persMode
            if(pers_mode) {
                new_block.data[0] &= rx_data[2]; // Applications Limit
                new_block.data[4] &= rx_data[6]; // Chip Config
                new_block.data[5] &= rx_data[7]; // Memory Config
                new_block.data[7] &= rx_data[9]; // Fuses
            } else {
                // Fuses allows setting Crypt1/0 from 1 to 0 only during application mode
                new_block.data[7] &= rx_data[9] | ~PICOPASS_FUSE_CRYPT10;
            }
            break;

        case PICOPASS_SECURE_EPURSE_BLOCK_INDEX:
            if(secured) {
                // ePurse updates swap first and second half of the block each update on secure cards
                memcpy(&new_block.data[4], &rx_data[2], 4);
                memcpy(&new_block.data[0], &rx_data[6], 4);
            }
            break;

        case PICOPASS_SECURE_KD_BLOCK_INDEX:
            // fallthrough
        case PICOPASS_SECURE_KC_BLOCK_INDEX:
            if(!pers_mode && secured) {
                new_block = instance->data->AA1[block_num];
                for(size_t i = 0; i < sizeof(PicopassBlock); i++) {
                    new_block.data[i] ^= rx_data[i + 2];
                }
                break;
            }
            // Use default case when in personalisation mode
            // fallthrough
        default:
            memcpy(new_block.data, &rx_data[2], sizeof(PicopassBlock));
            break;
        }

        instance->data->AA1[block_num] = new_block;
        if(secured && ((block_num == instance->key_block_num) ||
                       (block_num == PICOPASS_SECURE_EPURSE_BLOCK_INDEX))) {
            picopass_listener_init_cipher_state(instance);
        }

        bit_buffer_reset(instance->tx_buffer);
        if(secured && ((block_num == PICOPASS_SECURE_KD_BLOCK_INDEX) ||
                       (block_num == PICOPASS_SECURE_KC_BLOCK_INDEX))) {
            // Key updates always return FF's
            for(size_t i = 0; i < PICOPASS_BLOCK_LEN; i++) {
                bit_buffer_append_byte(instance->tx_buffer, 0xff);
            }
        } else {
            bit_buffer_copy_bytes(
                instance->tx_buffer, instance->data->AA1[block_num].data, sizeof(PicopassBlock));
        }

        PicopassError error = picopass_listener_send_frame(instance, instance->tx_buffer);
        if(error != PicopassErrorNone) {
            FURI_LOG_D(TAG, "Failed to tx update response: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

PicopassListenerCommand
    picopass_listener_read4_handler(PicopassListener* instance, BitBuffer* buf) {
    PicopassListenerCommand command = PicopassListenerCommandSilent;

    do {
        if(instance->state != PicopassListenerStateSelected) break;

        uint8_t block_start = bit_buffer_get_byte(buf, 1);
        if(block_start + 4 >= PICOPASS_MAX_APP_LIMIT) break;

        bool secured = (instance->data->AA1[PICOPASS_CONFIG_BLOCK_INDEX].data[7] &
                        PICOPASS_FUSE_CRYPT10) != PICOPASS_FUSE_CRYPT0;

        // TODO: Check CRC?
        // TODO: Check auth?

        bit_buffer_reset(instance->tx_buffer);
        for(uint8_t i = block_start; i < block_start + 4; i++) {
            if(secured &&
               ((i == PICOPASS_SECURE_KD_BLOCK_INDEX) || (i == PICOPASS_SECURE_KC_BLOCK_INDEX))) {
                for(size_t j = 0; j < sizeof(PicopassBlock); j++) {
                    bit_buffer_append_byte(instance->tx_buffer, 0xff);
                }
            } else {
                bit_buffer_append_bytes(
                    instance->tx_buffer, instance->data->AA1[i].data, sizeof(PicopassBlock));
            }
        }

        PicopassError error = picopass_listener_send_frame(instance, instance->tx_buffer);
        if(error != PicopassErrorNone) {
            FURI_LOG_D(TAG, "Failed to tx read4 response: %d", error);
            break;
        }

        command = PicopassListenerCommandProcessed;
    } while(false);

    return command;
}

static const PicopassListenerCmd picopass_listener_cmd_handlers[] = {
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_ACTALL,
        .cmd_len_bits = 8,
        .handler = picopass_listener_actall_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_ACT,
        .cmd_len_bits = 8,
        .handler = picopass_listener_act_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_HALT,
        .cmd_len_bits = 8,
        .handler = picopass_listener_halt_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_READ_OR_IDENTIFY,
        .cmd_len_bits = 8,
        .handler = picopass_listener_identify_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_SELECT,
        .cmd_len_bits = 8 * 9,
        .handler = picopass_listener_select_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_READ_OR_IDENTIFY,
        .cmd_len_bits = 8 * 4,
        .handler = picopass_listener_read_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_READCHECK_KD,
        .cmd_len_bits = 8 * 2,
        .handler = picopass_listener_readcheck_kd_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_READCHECK_KC,
        .cmd_len_bits = 8 * 2,
        .handler = picopass_listener_readcheck_kc_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_CHECK,
        .cmd_len_bits = 8 * 9,
        .handler = picopass_listener_check_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_UPDATE,
        .cmd_len_bits = 8 * 14,
        .handler = picopass_listener_update_handler,
    },
    {
        .start_byte_cmd = RFAL_PICOPASS_CMD_READ4,
        .cmd_len_bits = 8 * 4,
        .handler = picopass_listener_read4_handler,
    },
    // TODO: RFAL_PICOPASS_CMD_DETECT
};

PicopassListener* picopass_listener_alloc(Nfc* nfc, const PicopassDeviceData* data) {
    furi_assert(nfc);
    furi_assert(data);

    PicopassListener* instance = malloc(sizeof(PicopassListener));
    instance->nfc = nfc;
    instance->data = malloc(sizeof(PicopassDeviceData));
    mempcpy(instance->data, data, sizeof(PicopassDeviceData));

    instance->tx_buffer = bit_buffer_alloc(PICOPASS_LISTENER_BUFFER_SIZE_MAX);
    instance->tmp_buffer = bit_buffer_alloc(PICOPASS_LISTENER_BUFFER_SIZE_MAX);

    nfc_set_fdt_listen_fc(instance->nfc, PICOPASS_FDT_LISTEN_FC);
    nfc_config(instance->nfc, NfcModeListener, NfcTechIso15693);

    return instance;
}

void picopass_listener_free(PicopassListener* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->tmp_buffer);
    free(instance->data);
    if(instance->writer) {
        loclass_writer_write_start_stop(instance->writer, false);
        loclass_writer_free(instance->writer);
    }
    free(instance);
}

bool picopass_listener_set_mode(PicopassListener* instance, PicopassListenerMode mode) {
    furi_assert(instance);
    bool success = true;

    instance->mode = mode;
    if(instance->mode == PicopassListenerModeLoclass) {
        instance->key_block_num = 0;
        picopass_listener_loclass_update_csn(instance);
        instance->writer = loclass_writer_alloc();
        if(instance->writer) {
            loclass_writer_write_start_stop(instance->writer, true);
        } else {
            success = false;
        }
    }

    return success;
}

NfcCommand picopass_listener_start_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcCommand command = NfcCommandContinue;
    PicopassListener* instance = context;
    BitBuffer* rx_buf = event.data.buffer;

    PicopassListenerCommand picopass_cmd = PicopassListenerCommandSilent;
    if(event.type == NfcEventTypeRxEnd) {
        for(size_t i = 0; i < COUNT_OF(picopass_listener_cmd_handlers); i++) {
            if(bit_buffer_get_size(rx_buf) != picopass_listener_cmd_handlers[i].cmd_len_bits) {
                continue;
            }
            if(bit_buffer_get_byte(rx_buf, 0) !=
               picopass_listener_cmd_handlers[i].start_byte_cmd) {
                continue;
            }
            picopass_cmd = picopass_listener_cmd_handlers[i].handler(instance, rx_buf);
            break;
        }
        if(picopass_cmd == PicopassListenerCommandSendSoF) {
            nfc_iso15693_listener_tx_sof(instance->nfc);
        } else if(picopass_cmd == PicopassListenerCommandStop) {
            command = NfcCommandStop;
        }
    }

    return command;
}

void picopass_listener_start(
    PicopassListener* instance,
    PicopassListenerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    picopass_listener_reset(instance);
    nfc_start(instance->nfc, picopass_listener_start_callback, instance);
}

void picopass_listener_stop(PicopassListener* instance) {
    furi_assert(instance);

    nfc_stop(instance->nfc);
}

const PicopassDeviceData* picopass_listener_get_data(PicopassListener* instance) {
    furi_assert(instance);

    return instance->data;
}
