#include <rfal_t1t.h>
#include "topaz.h"

#define TAG "Topaz"

bool topaz_check_card_type(uint8_t ATQA0, uint8_t ATQA1) {
    return ATQA0 == 0x00 && ATQA1 == 0x0C;
}

TopazType topaz_get_type_from_hr0(uint8_t hr0) {
    if(hr0 == TOPAZ_96_HR0) {
        return TopazType96;
    } else if(hr0 == TOPAZ_512_HR0) {
        return TopazType512;
    } else {
        return TopazTypeUnknown;
    }
}

size_t topaz_get_size_by_type(TopazType type) {
    if(type == TopazType96) {
        return TOPAZ_96_SIZE;
    } else if(type == TopazType512) {
        return TOPAZ_512_SIZE;
    } else {
        return TOPAZ_96_SIZE; // Probably safe fallback size; What's the size of Jewel?
    }
}

bool topaz_read_card(FuriHalNfcTxRxContext* tx_rx, TopazData* data, uint8_t* uid) {
    size_t data_read = 0;
    bool read_success = false;

    do {
        if(rfalT1TPollerInitialize() != ERR_NONE) {
            FURI_LOG_D(TAG, "Failed to initialize poller");
            break;
        }

        // Perform RALL: HR0, HR1, and block 00-0E
        tx_rx->tx_data[0] = TOPAZ_CMD_RALL;
        tx_rx->tx_data[1] = 0;
        tx_rx->tx_data[2] = 0;
        memcpy(&tx_rx->tx_data[3], uid, 4);
        tx_rx->tx_bits = 7 * 8;
        tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

        if(!furi_hal_nfc_tx_rx(tx_rx, 50) || tx_rx->rx_bits != 122 * 8) {
            FURI_LOG_D(TAG, "Failed to RALL");
            break;
        }

        memcpy(data->hr, tx_rx->rx_data, sizeof(data->hr));

        data->type = topaz_get_type_from_hr0(data->hr[0]);
        if(data->type == TopazTypeUnknown) {
            FURI_LOG_D(TAG, "Unknown type, HR0=%#04x", data->hr[0]);
            break;
        }
        data->size = topaz_get_size_by_type(data->type);

        memcpy(data->data, &tx_rx->rx_data[2], TOPAZ_BYTES_RALL);
        data_read += TOPAZ_BYTES_RALL;

        if(data->size > TOPAZ_96_SIZE) {
            // Perform READ8 on block 0F
            tx_rx->tx_data[0] = TOPAZ_CMD_READ8;
            tx_rx->tx_data[1] = 0x0F;
            memset(&tx_rx->tx_data[2], 0, TOPAZ_BYTES_PER_BLOCK);
            memcpy(&tx_rx->tx_data[10], uid, 4);
            tx_rx->tx_bits = 14 * 8;
            tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

            if(!furi_hal_nfc_tx_rx(tx_rx, 50) || tx_rx->rx_bits != 9 * 8 ||
               tx_rx->rx_data[0] != tx_rx->tx_data[1]) {
                FURI_LOG_D(TAG, "Failed to read block 0F");
                break;
            }

            memcpy(&data->data[data_read], &tx_rx->rx_data[1], TOPAZ_BYTES_PER_BLOCK);
            data_read += TOPAZ_BYTES_PER_BLOCK;

            // Perform RSEG for rest of tag
            for(uint8_t i = 1; data_read < data->size; ++i) {
                tx_rx->tx_data[0] = TOPAZ_CMD_RSEG;
                tx_rx->tx_data[1] = i << 4;
                memset(&tx_rx->tx_data[2], 0, TOPAZ_BYTES_PER_BLOCK);
                memcpy(&tx_rx->tx_data[10], uid, 4);
                tx_rx->tx_bits = 14 * 8;
                tx_rx->tx_rx_type = FuriHalNfcTxRxTypeDefault;

                if(!furi_hal_nfc_tx_rx(tx_rx, 50) || tx_rx->rx_bits != 129 * 8 ||
                   tx_rx->rx_data[0] != tx_rx->tx_data[1]) {
                    FURI_LOG_D(TAG, "Failed to read segment %d", i);
                    break;
                }

                memcpy(&data->data[data_read], &tx_rx->rx_data[1], TOPAZ_BYTES_PER_SECTOR);
                data_read += TOPAZ_BYTES_PER_SECTOR;
            }
        }

        read_success = true;
    } while(false);

    return read_success;
}

static bool topaz_emulation_check_uid(TopazData* data, uint8_t* uid) {
    // Although datasheet specifies the lower 4 bytes of UID are checked, actual tag only appears to check 3
    return data->data[0] == uid[0] && data->data[1] == uid[1] && data->data[2] == uid[2];
}

static bool topaz_emulation_check_block_valid(TopazData* data, uint8_t add8) {
    uint8_t num_blocks = data->size / TOPAZ_BYTES_PER_BLOCK;
    return add8 < num_blocks;
}

static bool topaz_emulation_check_segment_valid(TopazData* data, uint8_t adds) {
    uint8_t num_segments = data->size / TOPAZ_BYTES_PER_SECTOR;
    // The equals 0 case is needed because Topaz 96 does not fill a full block
    return adds < num_segments || adds == 0;
}

void topaz_prepare_emulation(TopazEmulator* emulator, TopazData* data) {
    emulator->data = *data;
    emulator->data_changed = false;
    memset(emulator->buff_rx, 0, sizeof(emulator->buff_rx));
    emulator->buff_level = 0;
}

static bool topaz_emulation_process_command(
    TopazEmulator* emulator,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len) {
    bool command_parsed = false;
    TopazData* data = &emulator->data;

    int8_t cmd_len = emulator->buff_level - 2;
    uint8_t cmd = emulator->buff_rx[0];
    if(cmd_len == 7) {
        // Basic Topaz commands
        if(cmd == TOPAZ_CMD_RID) {
            // Operand frames not checked
            memcpy(&buff_tx[0], data->hr, sizeof(data->hr));
            memcpy(&buff_tx[2], data->data, 4);
            *buff_tx_len = 6;
            command_parsed = true;
        } else if(cmd == TOPAZ_CMD_RALL) {
            if(topaz_emulation_check_uid(data, &emulator->buff_rx[3])) {
                // Address and data frames not checked
                memcpy(&buff_tx[0], data->hr, sizeof(data->hr));
                memcpy(&buff_tx[2], data->data, TOPAZ_BYTES_RALL);
                *buff_tx_len = 2 + TOPAZ_BYTES_RALL;
                command_parsed = true;
            }
        } else if(cmd == TOPAZ_CMD_READ) {
            if(topaz_emulation_check_uid(data, &emulator->buff_rx[3])) {
                // Address frame MSB is always zero
                uint8_t addr = emulator->buff_rx[1] & 0x7F;
                buff_tx[0] = addr;
                buff_tx[1] = data->data[addr];
                *buff_tx_len = 2;
                command_parsed = true;
            }
        }
    } else if(cmd_len == 14) {
        // Extended Topaz commands, available as of Topaz 512
        if(cmd == TOPAZ_CMD_RSEG) {
            if(topaz_emulation_check_uid(data, &emulator->buff_rx[10])) {
                uint8_t adds = emulator->buff_rx[1];
                // Data frames not checked
                buff_tx[0] = adds;
                if(topaz_emulation_check_segment_valid(data, adds)) {
                    memcpy(
                        &buff_tx[1],
                        &data->data[(adds >> 4) * TOPAZ_BYTES_PER_SECTOR],
                        TOPAZ_BYTES_PER_SECTOR);
                } else {
                    memset(&buff_tx[1], 0, TOPAZ_BYTES_PER_SECTOR);
                }
                *buff_tx_len = 1 + TOPAZ_BYTES_PER_SECTOR;
                command_parsed = true;
            }
        } else if(cmd == TOPAZ_CMD_READ8) {
            if(topaz_emulation_check_uid(data, &emulator->buff_rx[10])) {
                uint8_t add8 = emulator->buff_rx[1];
                // Data frames not checked
                buff_tx[0] = add8;
                if(topaz_emulation_check_block_valid(data, add8)) {
                    memcpy(
                        &buff_tx[1],
                        &data->data[add8 * TOPAZ_BYTES_PER_BLOCK],
                        TOPAZ_BYTES_PER_BLOCK);
                } else {
                    memset(&buff_tx[1], 0, TOPAZ_BYTES_PER_BLOCK);
                }
                *buff_tx_len = 9;
                command_parsed = true;
            }
        }
    }

    return command_parsed;
}

void topaz_emulation_reset(TopazEmulator* emulator) {
    emulator->buff_level = 0;
}

static bool topaz_emulation_is_cmd_valid(uint8_t cmd, uint8_t buff_level) {
    if(buff_level == 9 &&
       (cmd == TOPAZ_CMD_RID || cmd == TOPAZ_CMD_RALL || cmd == TOPAZ_CMD_READ ||
        cmd == TOPAZ_CMD_WRITE_E || cmd == TOPAZ_CMD_WRITE_NE)) {
        return true;
    } else if(
        buff_level == 16 && (cmd == TOPAZ_CMD_RSEG || cmd == TOPAZ_CMD_READ8 ||
                             cmd == TOPAZ_CMD_WRITE_E8 || cmd == TOPAZ_CMD_WRITE_NE8)) {
        return true;
    } else {
        return false;
    }
}

bool topaz_prepare_emulation_response(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* data_type,
    void* context) {
    furi_assert(context);
    TopazEmulator* emulator = context;
    bool command_parsed = false;
    bool suppress_postprocessing = false;

    furi_assert(buff_rx_len == 8);
    if(emulator->buff_level == 0 && (*buff_rx == RFAL_14443A_SHORTFRAME_CMD_WUPA ||
                                     *buff_rx == RFAL_14443A_SHORTFRAME_CMD_REQA)) {
        // I don't know why I'm getting this here
        buff_tx[0] = 0x00;
        buff_tx[1] = 0x0c;
        *buff_tx_len = 16;
        *data_type = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
        suppress_postprocessing = true;
    } else {
        if(emulator->buff_level < sizeof(emulator->buff_rx)) {
            FURI_LOG_D(TAG, "Received byte: %02x", *buff_rx);
            emulator->buff_rx[emulator->buff_level++] = *buff_rx;
        } else {
            FURI_LOG_D(TAG, "Receive buffer overflow, discarding");
            topaz_emulation_reset(emulator);
        }
    }

    if(emulator->buff_level == 9 || emulator->buff_level == 16) {
        uint8_t cmd = emulator->buff_rx[0];
        bool cmd_valid = false;
        do {
            if(!topaz_emulation_is_cmd_valid(cmd, emulator->buff_level)) break;
            uint16_t calc_crc = nfca_get_crc16(emulator->buff_rx, emulator->buff_level - 2);
            if(emulator->buff_rx[emulator->buff_level - 2] != (calc_crc & 0xff) ||
               emulator->buff_rx[emulator->buff_level - 1] != (calc_crc >> 8))
                break;
            cmd_valid = true;
        } while(false);

        if(cmd_valid) {
            FURI_LOG_D(TAG, "Valid command received (%02x), processing", cmd);
            command_parsed = topaz_emulation_process_command(emulator, buff_tx, buff_tx_len);
            topaz_emulation_reset(emulator);
        } else if(topaz_emulation_is_cmd_valid(cmd, 16)) {
            // Command should be valid, wait until we get all our bytes
        } else {
            FURI_LOG_D(TAG, "Invalid command");
            topaz_emulation_reset(emulator);
        }
    }

    if(!suppress_postprocessing) {
        if(command_parsed) {
            *buff_tx_len *= 8;
            *data_type = FURI_HAL_NFC_TXRX_DEFAULT;
        } else {
            // Tag does not return to IDLE state, so transmit nothing if there's no response
            *buff_tx_len = UINT16_MAX;
            *data_type = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
        }
    }

    return true;
}
