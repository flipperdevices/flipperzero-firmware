#include "nfc_supported_card.h"
#include "united.h"

#include <gui/modules/widget.h>
#include <nfc_worker_i.h>

#include "furi_hal.h"

#define UNITED_LAYOUT_UNKNOWN 0
#define UNITED_LAYOUT_A 1
#define UNITED_LAYOUT_D 2
#define UNITED_LAYOUT_E2 3
#define UNITED_LAYOUT_E3 4
#define UNITED_LAYOUT_E5 5
#define UNITED_LAYOUT_2 6

uint8_t united_get_layout(NfcWorker* nfc_worker) {
    // I absolutely hate what's about to happen here.

    // Switch on the second half of the third byte of page 5
    FURI_LOG_I("united", "Layout byte: %02x", nfc_worker->dev_data->mf_ul_data.data[(4 * 5) + 2]);
    FURI_LOG_I(
        "united",
        "Layout half-byte: %02x",
        nfc_worker->dev_data->mf_ul_data.data[(4 * 5) + 3] & 0x0F);
    switch(nfc_worker->dev_data->mf_ul_data.data[(4 * 5) + 2] & 0x0F) {
    // If it is A, the layout type is a type A layout
    case 0x0A:
        return UNITED_LAYOUT_A;
    case 0x0D:
        return UNITED_LAYOUT_D;
    case 0x02:
        return UNITED_LAYOUT_2;
    default:
        FURI_LOG_I(
            "united",
            "Unknown layout type: %d",
            nfc_worker->dev_data->mf_ul_data.data[(4 * 5) + 2] & 0x0F);
        return UNITED_LAYOUT_UNKNOWN;
    }
}

bool united_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    UNUSED(nfc_worker);
    // If this is a united pass, first 2 bytes of page 4 are 0x45 0xD9
    MfUltralightReader reader = {};
    MfUltralightData data = {};

    if(!mf_ul_read_card(tx_rx, &reader, &data)) {
        return false;
    } else {
        if(data.data[4 * 4] == 0x45 && data.data[4 * 4 + 1] == 0xD9) {
            FURI_LOG_I("united", "Pass verified");
            return true;
        }
    }
    return false;
}

bool united_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    MfUltralightReader reader = {};
    MfUltralightData data = {};
    if(!mf_ul_read_card(tx_rx, &reader, &data)) {
        return false;
    } else {
        memcpy(&nfc_worker->dev_data->mf_ul_data, &data, sizeof(data));
        FURI_LOG_I("united", "Card read");
        return true;
    }
}

bool united_parser_parse(NfcWorker* nfc_worker) {
    // If the layout is a then the ride count is stored in the first byte of page 8
    uint8_t ride_count = 0;
    uint32_t serial = 0;
    if(united_get_layout(nfc_worker) == UNITED_LAYOUT_A) {
        ride_count = nfc_worker->dev_data->mf_ul_data.data[4 * 8];
    } else if(united_get_layout(nfc_worker) == UNITED_LAYOUT_D) {
        // If the layout is D, the ride count is stored in the second byte of page 9
        ride_count = nfc_worker->dev_data->mf_ul_data.data[4 * 9 + 1];
        // I hate this with a burning passion.

        // The number starts at the second half of the third byte on page 4, and is 32 bits long
        // So we get the second half of the third byte, then bytes 4-6, and then the first half of the 7th byte
        // B8 17 A2 A4 BD becomes 81 7A 2A 4B
        serial = (nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 2] & 0x0F) << 28 |
                 nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 3] << 20 |
                 nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 4] << 12 |
                 nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 5] << 4 |
                 (nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 6] >> 4);
    } else {
        FURI_LOG_I("united", "Unknown layout: %d", united_get_layout(nfc_worker));
        ride_count = 137;
    }

    // I hate this with a burning passion.

    // The number starts at the second half of the third byte on page 4, and is 32 bits long
    // So we get the second half of the third byte, then bytes 4-6, and then the first half of the 7th byte
    // B8 17 A2 A4 BD becomes 81 7A 2A 4B
    serial = (nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 2] & 0x0F) << 28 |
             nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 3] << 20 |
             nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 4] << 12 |
             nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 5] << 4 |
             (nfc_worker->dev_data->mf_ul_data.data[4 * 4 + 6] >> 4);

    // Format string for rides count
    string_printf(
        nfc_worker->dev_data->parsed_data,
        "United Transport card\nNumber: %u\nRides left: %u",
        serial,
        ride_count);
    return true;
}