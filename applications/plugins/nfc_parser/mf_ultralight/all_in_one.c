#include "all_in_one.h"

#include <furi.h>

#define TAG "all in one"

#define ALL_IN_ONE_LAYOUT_UNKNOWN 0
#define ALL_IN_ONE_LAYOUT_A 1
#define ALL_IN_ONE_LAYOUT_D 2
#define ALL_IN_ONE_LAYOUT_E2 3
#define ALL_IN_ONE_LAYOUT_E3 4
#define ALL_IN_ONE_LAYOUT_E5 5
#define ALL_IN_ONE_LAYOUT_2 6

static uint8_t all_in_one_get_layout(MfUltralightData* data) {
    // I absolutely hate what's about to happen here.

    // Switch on the second half of the third byte of page 5
    FURI_LOG_I(TAG, "Layout byte: %02x", data->data[(4 * 5) + 2]);
    FURI_LOG_I(TAG, "Layout half-byte: %02x", data->data[(4 * 5) + 3] & 0x0F);
    switch(data->data[(4 * 5) + 2] & 0x0F) {
    // If it is A, the layout type is a type A layout
    case 0x0A:
        return ALL_IN_ONE_LAYOUT_A;
    case 0x0D:
        return ALL_IN_ONE_LAYOUT_D;
    case 0x02:
        return ALL_IN_ONE_LAYOUT_2;
    default:
        FURI_LOG_I(TAG, "Unknown layout type: %d", data->data[(4 * 5) + 2] & 0x0F);
        return ALL_IN_ONE_LAYOUT_UNKNOWN;
    }
}
bool all_in_one_verify(MfUltralightData* data) {
    furi_assert(data);
    
    bool verified = false;

    if(data->data[4 * 4] == 0x45 && data->data[4 * 4 + 1] == 0xD9) {
        FURI_LOG_I(TAG, "Pass verified");
        verified = true;
    }

    return verified;
}

bool all_in_one_parse(MfUltralightData* data, FuriString* parsed_data) {
    furi_assert(data);
    furi_assert(parsed_data);
    uint8_t ride_count = 0;
    uint32_t serial = 0;
    if(all_in_one_get_layout(data) == ALL_IN_ONE_LAYOUT_A) {
        // If the layout is A then the ride count is stored in the first byte of page 8
        ride_count = data->data[4 * 8];
    } else if(all_in_one_get_layout(data) == ALL_IN_ONE_LAYOUT_D) {
        // If the layout is D, the ride count is stored in the second byte of page 9
        ride_count = data->data[4 * 9 + 1];
    } else {
        FURI_LOG_I(TAG, "Unknown layout: %d", all_in_one_get_layout(data));
        ride_count = 137;
    }

    // I hate this with a burning passion.

    // The number starts at the second half of the third byte on page 4, and is 32 bits long
    // So we get the second half of the third byte, then bytes 4-6, and then the first half of the 7th byte
    // B8 17 A2 A4 BD becomes 81 7A 2A 4B
    serial = (data->data[4 * 4 + 2] & 0x0F) << 28 | data->data[4 * 4 + 3] << 20 |
             data->data[4 * 4 + 4] << 12 | data->data[4 * 4 + 5] << 4 |
             (data->data[4 * 4 + 6] >> 4);

    // Format string for rides count
    furi_string_printf(
        parsed_data, "\e#All-In-One\nNumber: %lu\nRides left: %u", serial, ride_count);
    return true;
}
