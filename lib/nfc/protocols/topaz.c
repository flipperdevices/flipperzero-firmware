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
